static void ReceiveDataFrame (avt_unit *punit, word *buf)
{
  int CurWordOfBlock, LastWordsOfBlock, NumWords;
  int SkipCounter = punit->WArvFrame * punit->HeadStr;
  int LastWords = punit->SizeArvFrame - SkipCounter;

  outw (punit->SubFrame++, RA);
  inw (RD); /* Arvid have gluck - point 2 of hints - that work */
  CurWordOfBlock = 0;
  LastWordsOfBlock = 0x100 - CurWordOfBlock;
  
  /* Scip header strings of frame */
  /* Two first strings - No0, No1 - have error rating 50% */
  while (SkipCounter)
   {
    if (SkipCounter >= LastWordsOfBlock)
     {
      outw (punit->SubFrame++, RA);
      inw (RD); /* Arvid have gluck - point 2 of hints - that work */

      CurWordOfBlock = 0;
      LastWordsOfBlock = 0x100 - CurWordOfBlock;
      SkipCounter -= LastWordsOfBlock;
     }
    else
     {
      insw (RD_INC, buf, SkipCounter);

      CurWordOfBlock = SkipCounter;
      LastWordsOfBlock = 0x100 - CurWordOfBlock;
      SkipCounter = 0;
     }
   }

  while (LastWords)
   {
    if (LastWords > LastWordsOfBlock)
     {
      NumWords = LastWordsOfBlock;
      LastWords -= LastWordsOfBlock;
     }
    else
     {
      NumWords = LastWords;
      LastWords = 0;
     }

    insw (RD_INC, buf, NumWords);
    buf += NumWords;

    if (LastWords)
     {
      CurWordOfBlock = 0x100;
      LastWordsOfBlock = 0x100 - CurWordOfBlock;
     }
    else
     {
      CurWordOfBlock = NumWords;
      LastWordsOfBlock = 0x100 - CurWordOfBlock;
      break; /* dangerous break - check addition to punit->SubFrame
      needs +1 to number */
     }

    outw (punit->SubFrame++, RA);
    inw (RD); /* Arvid have gluck - point 2 of hints - that work */
    CurWordOfBlock = 0;
    LastWordsOfBlock = 0x100 - CurWordOfBlock;
   }

  punit->ReceiveFrameCount++;
  outw (punit->ValueRK | SET_INC_BUFFER, RK); /* increment frame count */
}

static void SendDataFrame (avt_unit *punit, word *buf)
{
  int CurWordOfBlock = 1, LastWordsOfBlock, NumWords; /* (?->KSI) */
  int SkipCounter = punit->WArvFrame * punit->HeadStr - (CurWordOfBlock + punit->ResetSeqFlag);
  int LastWords = punit->SizeArvFrame - SkipCounter;

  outw (punit->SubFrame++, RA);
  CurWordOfBlock += punit->ResetSeqFlag; /* (?->KSI) */
  LastWordsOfBlock = 0x100 - CurWordOfBlock;
  punit->ResetSeqFlag = 0; /* reset start sequential trigger */

  /* Scip header strings of frame */
  /* Two first strings - No0, No1 - have error rating 50% */
  while (SkipCounter)
   {
    if (SkipCounter >= LastWordsOfBlock)
     {
      outw (punit->SubFrame++, RA);
      CurWordOfBlock = 0;
      LastWordsOfBlock = 0x100 - CurWordOfBlock;

      SkipCounter -= LastWordsOfBlock;
     }
    else
     {
      outsw (RD_INC, buf, SkipCounter);
      CurWordOfBlock = SkipCounter;
      LastWordsOfBlock = 0x100 - CurWordOfBlock;

      SkipCounter = 0;
     }
   }

  while (LastWords)
   {
    if (LastWords > LastWordsOfBlock)
     {
      NumWords = LastWordsOfBlock;
      LastWords -= LastWordsOfBlock;
     }
    else
     {
      NumWords = LastWords;
      LastWords = 0;
     }

    outsw (RD_INC, buf, NumWords);
    buf += NumWords;

    if (LastWords)
     {
      CurWordOfBlock = 0x100;
      LastWordsOfBlock = 0x100 - CurWordOfBlock;
     }
    else
     {
      CurWordOfBlock = NumWords;
      LastWordsOfBlock = 0x100 - CurWordOfBlock;
      break; /* dangerous break - check addition to punit->SubFrame
      needs +1 to number */
     }

    outw (punit->SubFrame++, RA);
    CurWordOfBlock = 0;
    LastWordsOfBlock = 0x100 - CurWordOfBlock;
   }

  punit->SendFrameCount++;
  outw (punit->ValueRK | SET_INC_BUFFER, RK); /* increment frame count */
}
