#include "compiler.h"
#include <_dpmi.h>

#include "mytypes.h"
#include "adriver.h"
#include "dpmiserv.h"
#include "console.h"
#include "mtime.h"
#include "aftp.h"
#include "crc.h"

int SendFile (char *FileName)
{
  WaitLoop (DelayToTransmit);

  int hFile;
  if ((hFile = open (FileName, O_BINARY | O_RDONLY, FILE_ACCESS)) == -1)
   {
    printf ("-Send file %s not found!\n", FileName);
    return hFile;
   }

  dword FileSize = lseek (hFile, 0, SEEK_END), FileOffset = 0;
  lseek (hFile, 0, SEEK_SET);
  byte *ClusterBuffer = new byte [SizeOrigCluster];

  if (!ClusterBuffer)
    return (int)ClusterBuffer;

  LockMemory (ClusterBuffer, SizeOrigCluster, LOCK_MEMORY);
  // Make file header
  FileHdr *filehdr = (FileHdr*)ClusterBuffer;
  filehdr->FileID  = get_time () + FileSize; // Is static parametr

  dword FileHeaderLen = strlen (FileName) + 1 + sizeof (FileAttr);
  dword FileBlockIndex = 0;

  // Insert empty frames
  vint Semaphore = SizeOrigCluster / SizeDataFrame;
  SendAFrames ((word*)ClusterBuffer, &Semaphore, ID_FRAME, NULL);
  while (Semaphore > 4) {}

  while (FileSize && !StopByUser)
   {
    CheckOnFreeComBuffer ();
    BlockHdr *pBH = (BlockHdr*) ((int)ClusterBuffer + sizeof (FileHdr));
    dword BlockIndex = FileBlockIndex, BlockInCluster = 0;
    dword LastBytes = SizeOrigCluster - sizeof (FileHdr);

    // Divide cluster on blocks
    while (LastBytes)
     {
      if (LastBytes <= sizeof (BlockHdr))
	break;

      BlockInCluster++;
      LastBytes -= sizeof (BlockHdr);

      if (BlockIndex)
       {
	if (LastBytes >= BLOCK_LEN)
	  pBH->Len = BLOCK_LEN;
	else
	  pBH->Len = LastBytes;

	LastBytes -= pBH->Len;
       }
      else
      if (FileHeaderLen <= LastBytes)
       {
	pBH->Len = FileHeaderLen;
	LastBytes -= pBH->Len;
       }
      else
       {
	BlockInCluster--;
	LastBytes = 0;
	break;
       }

      BlockIndex++;
      pBH++;
     }

    // Dinamyc part of file header modify
    filehdr->NumBlock = BlockInCluster;
    filehdr->HdrLen = sizeof (FileHdr) + BlockInCluster * sizeof (BlockHdr);
    byte *pClusterData = (byte*)((int)ClusterBuffer + filehdr->HdrLen);
    pBH = (BlockHdr *) ((int)ClusterBuffer + sizeof (FileHdr));
    dword OffsetToData = filehdr->HdrLen;

    BlockIndex = 0;
    // Skeleton of cluster data fill
    while (BlockInCluster && FileSize)
     {
      BlockIndex++;

      if (!FileBlockIndex)
       {
	FileAttr *pFA = (FileAttr *)pClusterData;
	pFA->Size = FileSize;
	_dos_getfileattr (FileName, &pFA->Attr);
#ifdef __GNUC__
	_dos_getftime (hFile, &pFA->Date, &pFA->Time);
#else
	_dos_getftime (hFile, (word*)&pFA->Date, (word*)&pFA->Time);
#endif
	strcpy ((char*) ((int)pClusterData + sizeof (FileAttr)), FileName);
       }
      else
       {
	read (hFile, pClusterData, pBH->Len);
	pBH->FileOffset = FileOffset;
	FileOffset += pBH->Len;

	if (FileSize >= pBH->Len)
	  FileSize -= pBH->Len;
	else
	 {
	  pBH->Len = FileSize; // truncate block
	  FileSize = 0;
	  // Correct file header
	  filehdr->NumBlock = BlockIndex;
	  filehdr->HdrLen = sizeof (FileHdr) + BlockIndex * sizeof (BlockHdr);
	 }
       }

      pBH->Number = FileBlockIndex++;
      pBH->CRC = GetBlockCRC (pClusterData, pBH->Len);
      pBH->ClusterOffset = OffsetToData;

      OffsetToData += pBH->Len;
      pClusterData += pBH->Len;
      BlockInCluster--;
      pBH++;
     }

    printf ("BlockInCluster %d\n", BlockIndex);
    dword CountCluster;
    while ((CountCluster = SendACluster (1, ClusterBuffer, filehdr->FileID + FileBlockIndex)) != 1) {}
   } // end while FileSize

  delete ClusterBuffer;
  LockMemory (ClusterBuffer, SizeOrigCluster, UNLOCK_MEMORY);
  close (hFile);

  while ((SComFlag || RComFlag || StartFlag) && !StopByUser)
    CheckOnFreeComBuffer ();
  return 1;
}

int ReceiveFile (char *FileName)
{
  dword FileID = 0, FileSize = 0, CurSize = 0;
  bool FileNotReceive = true;
  int hFile = -1;

  ReceiveACluster (LEN_SEQ); // Init receive

  if (ErrAudit)
    printf ("|--Cluster--|-----I----|----II----|----III---|--Summary--|\n");

  dword CurBlock = 0;
  while (FileNotReceive && !StopByUser)
   {
    CheckOnFreeComBuffer ();

    for (dword i = 0; i < MAX_AS_COM; i++)
     {
      if (ComTable [i].Semaphore == END_DEC_COM)
       {
	byte *ClusterBuffer = (byte*)ComTable [i].Buffer;
	FileHdr *filehdr = (FileHdr*)ClusterBuffer;

	if (CurBlock && filehdr->FileID != FileID)
	 {
	  //brk ();
	  continue; // Go to next cluster
	 }

	bool ClusterNotEmpty = true;
	dword CountBlock = 0;

	while (ClusterNotEmpty && FileNotReceive)
	 {
	  BlockHdr *pBH = (BlockHdr*) ((dword)ClusterBuffer + sizeof (FileHdr));

	  for (dword j = 0; j < filehdr->NumBlock; j++, pBH++)
	    if (pBH->Number == CurBlock)
	      break; // end loop

	  // next block not found in current cluster
	  if (pBH->Number != CurBlock)
	    break; // Go to next cluster

	  // find first block our file by name in file attributs
	  if (!pBH->Number)
	   {
	    char *ArchiveName = (char*) ((dword)ClusterBuffer + pBH->ClusterOffset + sizeof (FileAttr));
	    FileAttr *fattr = (FileAttr*) ((dword)ClusterBuffer + pBH->ClusterOffset);
	    // Other file - skip
//	      if (stricmp (ArchiveName, FileName));
//	       {
//		brk ();
//		break; // Go to next cluster
//	       }

	    char NameBuff [128];
	    strcpy (NameBuff, ArchiveName);
	    strcat (NameBuff, ".out");

	    // Create file
	    if ((hFile = open (NameBuff, O_CREAT | O_BINARY | O_WRONLY, 0222)) == -1)
	      return hFile;

	    _dos_setfileattr (NameBuff, fattr->Attr);
	    _dos_setftime (hFile, fattr->Date, fattr->Time);

	    FileID = filehdr->FileID; // Set hook variable
	    FileAttr *pFA = (FileAttr*)((dword)ClusterBuffer + pBH->ClusterOffset);
	    FileSize = pFA->Size;
	   }
	  else
	   {
	    printf ("Receive block num:%6d, len:%6d\n", pBH->Number, pBH->Len);
	    fflush (stdout);
	    write (hFile, (byte*)((dword)ClusterBuffer + pBH->ClusterOffset), pBH->Len);
	    CurSize += pBH->Len;
	   }

	  CurBlock++;
	  CountBlock++;

	  if (CountBlock == filehdr->NumBlock)
	   {
	    ClusterNotEmpty = false;
	    ReceiveACluster (1);
	   }

	  if (FileSize == CurSize)
	   {
	    close (hFile);
	    FileNotReceive = false;
	    ClusterNotEmpty = false;
	   }
	 } // End while cluster not empty

	if (!ClusterNotEmpty && !StopByUser)
	 {
	  if (ErrAudit)
	   {
	    printf ("| %8d | %8d | %8d | %8d |	%8d |\n", i,
		    ComTable [i].ES.OrdinaryErr,
		    ComTable [i].ES.DupletErr,
		    ComTable [i].ES.TripletErr,
		    ComTable [i].ES.OrdinaryErr +
		   (ComTable [i].ES.DupletErr * 2) +
		   (ComTable [i].ES.TripletErr * 3));
	   }

	  FreeSemaphor (i);
	 }
       } // end if find END_DECODE_COMMAND
     } // end for
   } // End while FileNotReceive

  PrintRFStatisics ();
  return 1;
}
