
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <linux/errno.h>
#include <gtk/gtk.h>

#include "defines.h"
#include "arvid.h"
#include "avd_ioctl.h"

#define VERBOSE		1    /* messaging output level */
#define NO_VERBOSE	0

/* used for frames exchange in queues */
#define MAX_FRAME_SIZE	8000 /* length commands data */

static char CURRENT_MODE = 'h';

static char* frame_buffer = NULL, terminate_thread = 0;

static long no_cache = 0, avd_event = 0;
static char* prev_frame_buffer = NULL;

int avd_get_param (int hFile, avd_param *pavdp, int verbose_flag)
{
  int rc = ioctl (hFile, AVT_GET_PARAM, pavdp);
  
  if (rc)
   {
    printf ("Cann't make ioctl request GET_PARAM, err %d\n!", rc);
    return rc;
   }    
  
  if (NO_VERBOSE == verbose_flag)
    return rc;
  
  printf ("Hardware parametrs:\n"
          "ArvidInsert: %i\n"
	  "ArvidModel: %i\n"
	  "ArvidModelDetect: %i\n"
	  "Phase: %Xh\n"
	  "SubPhase: %Xh\n"
	  "AutoPhase: %Xh\n"
	  "BasePort: %Xh\n"
	  "ArvidIRQ: %i\n"
          "ValueRK: %Xh\n"
	  "Arv105X: %Xh\n"
	  "DataMode: %Xh\n"
          "HalfDupMode: %Xh\n"
	  "HDensity: %Xh\n"
	  "FrameMask: %Xh\n"
	  "MemorySize: %i\n"
          "HArvFrame: %i\n"
	  "WArvFrame: %i\n"
	  "NumArvFrame: %i\n"
          "SizeArvFrame: %i\n\n"
          "countintr: %i\n"
          "SubFrame: %Xh\n"
          "SendFrameCount: %i\n"
          "ReceiveFrameCount: %i\n",
	  pavdp->ArvidInsert, pavdp->ArvidModel, pavdp->ArvidModelDetect, 
          pavdp->Phase, pavdp->SubPhase, pavdp->AutoPhase, pavdp->BasePort,
	  pavdp->ArvidIRQ, pavdp->ValueRK, pavdp->Arv105X, pavdp->DataMode,
	  pavdp->HalfDupMode, pavdp->HDensity, pavdp->FrameMask,
	  pavdp->MemorySize, pavdp->HArvFrame, pavdp->WArvFrame,
	  pavdp->NumArvFrame, pavdp->SizeArvFrame, pavdp->countintr,
	  pavdp->SubFrame, pavdp->SendFrameCount, pavdp->ReceiveFrameCount);

  return rc;
}

static guint hTimer;
  
gint terminateEvent (GtkWidget *widget, GdkEvent *event, gpointer data)
{
  printf ("Terminate event\n");
  
  terminate_thread = 1;

  gtk_timeout_remove (hTimer);
  gtk_main_quit ();
  
  return FALSE;
}

gint TimeOutNotify (gpointer data)
{
  if (terminate_thread)
    return 0;

  gboolean ret_event;
  avd_event = 1;
  gtk_signal_emit_by_name (GTK_OBJECT((GtkWidget*)data), "expose-event", NULL, &ret_event);

  return 1;
}

static int width  = 320;
static int height = 240;

gboolean configDraw (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  if (event != NULL)
   {
    width  = event->width;
    height = event->height;
   }
   
  return TRUE;
}

static GdkGC* gc = NULL;
static GdkGC* clean_gc = NULL;
static gint refresh_all = 1;

#define GET_BIT(bit)	(1 << bit)

#define DO_BIT(bit)	\
{			\
  if (refresh_all	\
  || ((avd_word & GET_BIT(bit)) ^ (prev_word & GET_BIT(bit)) ) )\
    gdk_draw_rectangle (widget->window, (avd_word & GET_BIT(bit)) ? gc : clean_gc, 1, (x + bit) << 2, y2, 4, 2);\
}

gboolean eventDraw (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  gint max_x = ((CURRENT_MODE == 'l') ? (WFRAME_1031 * 16) : (WFRAME_105X * 16));
  gint max_y = ((CURRENT_MODE == 'l') ? (HFRAME_1031) : (HFRAME_105X));
  gint x, y, y2, str_off;
  word avd_word, prev_word;
  
  if (!frame_buffer || !widget || terminate_thread)
    return FALSE;
    
  if (no_cache || !avd_event)
    refresh_all = 1;
    
  avd_event = 0;
    
  for (str_off = 0, y = 0; y < max_y && terminate_thread == 0; y++, str_off += max_x)
   {
    y2 = y << 1;

    for (x = 0; x < max_x && terminate_thread == 0; x += 16)
     {
      prev_word = ((word*)prev_frame_buffer) [(str_off + x) >> 4];
      avd_word  = ((word*)frame_buffer) [(str_off + x) >> 4];
      DO_BIT (0);
      DO_BIT (1);
      DO_BIT (2);
      DO_BIT (3);
      DO_BIT (4);
      DO_BIT (5);
      DO_BIT (6);
      DO_BIT (7);
      DO_BIT (8);
      DO_BIT (9);
      DO_BIT (10);
      DO_BIT (11);
      DO_BIT (12);
      DO_BIT (13);
      DO_BIT (14);
      DO_BIT (15);
     }
   }

  if (!no_cache)
    memcpy (prev_frame_buffer, frame_buffer, MAX_FRAME_SIZE);
    
  refresh_all = 0;

  return FALSE;
}

GdkGC* colorGC (GtkWidget* widget, gint red, gint green, gint blue)
{
  GdkColor* color;
  GdkGC* gc;

  /* first, create a GC to draw on */
  gc = gdk_gc_new (widget->window);

  /* the color we want to use */
  color = (GdkColor*) malloc (sizeof(GdkColor));
  
  /* red, green, and blue are passed values, indicating the RGB triple
   * of the color we want to draw. Note that the values of the RGB components
   * within the GdkColor are taken from 0 to 65535, not 0 to 255.
   */
  color->red   = red   * (65535/255);
  color->green = green * (65535/255);
  color->blue  = blue  * (65535/255);
  
  /* the pixel value indicates the index in the colormap of the color.
   * it is simply a combination of the RGB values we set earlier
   */
  color->pixel = (gulong)(red * 65536 + green * 256 + blue);

  /* However, the pixel valule is only truly valid on 24-bit (TrueColor)
   * displays. Therefore, this call is required so that GDK and X can
   * give us the closest color available in the colormap
   */
  gdk_color_alloc (gtk_widget_get_colormap(widget), color);

  /* set the foreground to our color */
  gdk_gc_set_foreground (gc, color);

  return gc;
}

static int hMem = ERROR;

int MainThread (int argc, char *argv[])
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget* window;
  GtkWidget* frame;
  GtkWidget* area;
  
  if ((frame_buffer = shmat (hMem, 0, 0)) <= 0)
   {
    printf ("MT:Can'nt attach to shared memory.\n");
    return -ENOMEM;
   }
   
  if (!(prev_frame_buffer = malloc (MAX_FRAME_SIZE)))
   {
    printf ("MT:Can'nt alloc memory.\n");
    return -ENOMEM;
   }
   
  memset (prev_frame_buffer, 0, MAX_FRAME_SIZE);
  
  /* Initialise GTK */
  gtk_init (&argc, &argv);

  /* Create a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Arvid Config Utility");

  /* Here we connect the "destroy" event to a signal handler */ 
  gtk_signal_connect (GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (terminateEvent), NULL);

  switch (CURRENT_MODE)
   {
    case 'l':
      width  = (WFRAME_1031 * 16) * 4;
      height = (HFRAME_1031) * 2;
      break;
    case 'h':
      width  = (WFRAME_105X * 16) * 4;
      height = (HFRAME_105X) * 2;
      break;
   }
   
  gtk_widget_set_usize (window, width, height);

  /* Sets the border width of the window. */
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Create a Frame */
  frame = gtk_frame_new (NULL);
  gtk_container_add (GTK_CONTAINER (window), frame);

  /* Set the frame's label */
  gtk_frame_set_label (GTK_FRAME (frame), CURRENT_MODE == 'l' ? "Arvid RAW data: LOW density" : "Arvid RAW data: HIGH density");

  /* Set the style of the frame */
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);

  area = gtk_drawing_area_new ();
  
  gtk_container_add  (GTK_CONTAINER (frame), area);
  gtk_signal_connect (GTK_OBJECT (area), "expose-event",    GTK_SIGNAL_FUNC (eventDraw),  NULL);
  gtk_signal_connect (GTK_OBJECT (area), "configure-event", GTK_SIGNAL_FUNC (configDraw), NULL);

  gtk_widget_show (area);
  gtk_widget_show (frame);
  gtk_widget_show (window);
  
  gc       = colorGC (area, 0xC0, 0xC0, 0xFF);
  clean_gc = colorGC (area, 0, 0, 0);
  
  hTimer = gtk_timeout_add (no_cache ? 100 : 20, TimeOutNotify, area);
  
  /* Enter the event loop */
  gtk_main ();
  
  gdk_gc_unref (gc);
  gdk_gc_unref (clean_gc);

  free (prev_frame_buffer);
  shmdt (frame_buffer);
  
  printf ("All is ok\n");  
  return 0;
}

int avd_read_fields (char *dev_path)
{
  avd_param avdp;
  int rc = 0, hFile, hOut;

  hOut  = open ("test.out", O_WRONLY | O_CREAT | O_TRUNC,FILE_ACCESS);
  hFile = open (dev_path,   O_RDONLY, FILE_ACCESS);
  
  if (hFile == -1 || hOut == -1)
   {
    if (hFile != -1)
      close (hFile);

    printf ("DT:Cann't open device file %s!\n", dev_path);
    return 1;
   }
   
  rc = avd_get_param (hFile, &avdp, NO_VERBOSE);
   
  if (rc)
   {
    close (hFile);
    close (hOut);

    return rc;
   }    
  
  int frame_buffer_size = avdp.SizeArvFrame * 2;
//  frame_buffer = malloc (frame_buffer_size);

  if ((frame_buffer = shmat (hMem, 0, 0)) <= 0)
   {
    printf ("DT:Can'nt attach to shared memory.\n");
    close (hFile);
    close (hOut);

    return -ENOMEM;
   }
   
  if ((rc = ioctl (hFile, AVT_SET_PHASE, 3)))
   {
    printf ("DT:Can'nt set phase.\n");
    close (hFile);
    close (hOut);

    return rc;
   }
  
  if ((rc = ioctl (hFile, AVT_SET_SUBPHASE, 1)))
   {
    printf ("DT:Can'nt set subphase.\n");
    close (hFile);
    close (hOut);

    return rc;
   }
  
  if ((rc = ioctl (hFile, AVT_SET_AUTOPHASE, 0)))
   {
    printf ("DT:Can'nt set autophase.\n");
    close (hFile);
    close (hOut);

    return rc;
   }
  
  while (!terminate_thread)
   {
    rc = read (hFile, frame_buffer, frame_buffer_size);

    if (rc != frame_buffer_size)
     {
      printf ("Cannot read %d bytes, return only %d bytes\n", frame_buffer_size, rc);

//    free (frame_buffer);
      shmdt (frame_buffer);
      
      if (shmctl (hMem, IPC_RMID, 0) < 0)
        printf ("DT:Cann't remove shared memory!\n");
       
      close (hFile);
      close (hOut);

      return -1;
     }
     
//  write (hOut, frame_buffer, rc > 0 ? rc : frame_buffer_size);
   }

  sleep (1);
  rc = avd_get_param (hFile, &avdp, VERBOSE);

//free (frame_buffer);
  shmdt (frame_buffer);
  
  if (shmctl (hMem, IPC_RMID, 0) < 0)
    printf ("DT:Cann't remove shared memory!\n");
  
  close (hFile);
  close (hOut);
  
  return rc;
}

static void exit_sighandler (int x)
{
  static int sig_count = 0;
  ++sig_count;
  
  if (sig_count == 2) 
    exit (1);
    
  if (sig_count > 2)
   {
    /* can't stop :( */
    kill (getpid(), SIGKILL);
   }

  terminate_thread = 1;
  exit (1);
}

/* Migrate from MPlayer */
static void SignalsHook (void)
{
  /* ========= Catch terminate signals: ================ */
  /* terminate requests: */
  signal (SIGTERM, exit_sighandler); // kill
  signal (SIGHUP,  exit_sighandler); // kill -HUP  /  xterm closed
  signal (SIGINT,  exit_sighandler); // Interrupt from keyboard
  signal (SIGQUIT, exit_sighandler); // Quit from keyboard
  signal (SIGCHLD, exit_sighandler); // Child dead
  
  /* fatal errors: */
  signal (SIGBUS,  exit_sighandler); // bus error
  signal (SIGSEGV, exit_sighandler); // segfault
  signal (SIGILL,  exit_sighandler); // illegal instruction
  signal (SIGFPE,  exit_sighandler); // floating point exc.
  signal (SIGABRT, exit_sighandler); // abort()
}

void *DriverThread (void *z)
{
  char dev_path [MAX_PATH], mode = CURRENT_MODE;
  int unit = 0;

  SignalsHook ();
  
  sprintf (dev_path, "/dev/%s%1i%c", MOD_NAME, unit, mode);
  printf ("dev_path: %s\n", dev_path);
    
  avd_read_fields (dev_path);
  printf ("End Thread\n");

  return NULL;
}

int main (int argc, char *argv[])
{
  key_t key = ftok ("/dev/avd0i", AVT_MAJOR);
  
  if (key < 0)
   {
    printf ("Main:Cann't get process key!\n");
    exit (1);
   }

  if ((hMem = shmget (key, MAX_FRAME_SIZE, 0)) >= 0)
   {
    printf ("Main:Shared memory alive! It will be destroyed!\n");
    
    if (shmctl (hMem, IPC_RMID, 0) < 0)
     {
      printf ("Main:Cann't remove shared memory!\n");
      exit (1);
     }
   }
   
  if ( (hMem = shmget (key, MAX_FRAME_SIZE, FILE_ACCESS | IPC_CREAT)) < 0)
   {
    printf ("Main:Cann't create shared memory!\n");
    exit (1);
   }
    
  printf ("Main:hMem %i\n", hMem);
   
  /* main thread split */
  pid_t pid = fork ();
  if (pid < 0)
   {
    perror ("fork");
    exit (1);
   }
  else
  if (!pid)
   {
    printf ("MT:Child alive!\n");
    return MainThread (argc, argv);
   }
  else
   {
    printf ("DT:Parent alive!\n");
    return (int) DriverThread (NULL);
   }
}
