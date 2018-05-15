/*
 * log.c
 *
 * ------
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Embedded Ocean, 2014
 */

#include <stdio.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

extern int verbose_level;

// log file name
static char log_filename[32];
static FILE *log_fp = NULL;

//
// Create and open a ascii log file
// Log file name is path/name.log
// Only on elog file can be open at a time.
//
int log_open(char *path, char *name)
{

	sprintf(log_filename, "%s/%s", path, name);

	// open/create an empty log file
	if((log_fp = fopen(log_filename, "w")) == NULL) {
		fprintf(stdout, "Error openning log file %s\n", log_filename);
		return(-1);
	}

	if(verbose_level)
		fprintf(stdout, "Logging all messages to %s\n", log_filename);

	return(1);

}

// Open a log file for append
// creates a new one if it doesn't exist
int log_open_append(char *path, char *name)
{

	sprintf(log_filename, "%s/%s", path, name);

	// open/create an empty log file
	if((log_fp = fopen(log_filename, "a+")) == NULL) {
		fprintf(stdout, "Error opening log file %s\n", log_filename);
		return(-1);
	}

	if(verbose_level)
		fprintf(stdout, "Logging all messages to %s\n", log_filename);

	return(1);

}

void log_close()
{
   if(log_fp == NULL) return;
   fclose(log_fp);
}

/*
* Write to a log file
* The data is attached to each formated sting 
*/
int log_printf(const char *format, ...)
{
   va_list arg;
   int done;
   FILE *fp;

   char tmstr[32]; // e.g. Tue Dec 21 22:10:50 2010
   struct timeval now;
   struct tm time;
   
   // use the opened log file, or stdout if no log file is open
   fp = log_fp;
   if(fp == NULL) fp = stdout;

   gettimeofday(&now, NULL);
   gmtime_r(&now.tv_sec, &time);
   
   strftime(tmstr, sizeof(tmstr), "%y/%m/%d %H:%M:%S", &time);

   fprintf(fp, "%s ", tmstr);

   va_start (arg, format);
   done = vfprintf (fp, format, arg);
   va_end (arg);

   fflush(fp);

   return done;

}
