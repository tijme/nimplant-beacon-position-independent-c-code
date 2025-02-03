/**
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###################                                         ###################
 * ################### THIS IS AN EXTERNAL LIBRARY FROM GITHUB ###################
 * ###################                                         ###################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * ###############################################################################
 * 
 * The author and owner of this specific JSON implementation is Valentin Dudouyt: 
 * https://github.com/vdudouyt. Copyright may apply. Please refer to the original 
 * source.
 * 
 * Original source: https://gist.github.com/vdudouyt/c7393c5cea7e8d58d0a8820b6f305edb
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

char lookup_escape_code(char c);
int json_escape(struct Relocatable* context, char *out, char *in, int in_len);

int jvsnprintf(struct Relocatable* context, char *out, size_t n, const char *format, va_list ap)
{
   context->functions.memset(out, 0, n);

   int ret = 0;

   int i;
   for(i = 0; i < context->functions.strlen(format); i++)
   {
      char *tail = &out[context->functions.strlen(out)];
      int remains = n - context->functions.strlen(out);

      if(format[i] == '\'')
      {
         *tail = '"';
         ret++;
      }
      else if(format[i] == '%')
      {
         char *str;
         int bytes_written;

         switch(format[i+1])
         {
            case '%':
               if(remains < 2) return -1;
               *tail = '%'; 
               ret++;
               break;
            case 'd':
               DEFINE_STRING(formatter, "%d");
               bytes_written = context->functions.snprintf(tail, remains-1, formatter, va_arg(ap, int));
               if(bytes_written < 0 || bytes_written > remains-1) return -1;
               ret += bytes_written;
               break;
            case 's':
               str = va_arg(ap, char *);
               if(remains < context->functions.strlen(str) * 6 + 2 + 1) return -1;

               DEFINE_STRING(quote, "\"");
               context->functions.strcat(tail, quote);
               bytes_written = json_escape(context, &tail[1], str, context->functions.strlen(str));
               context->functions.strcat(tail, quote);
               ret += bytes_written + 2;
               break;
            case 'b':
               DEFINE_STRING(falsestr, "false");
               DEFINE_STRING(truestr, "true");
               if(remains < context->functions.strlen(falsestr) + 1) return -1;
               char *end = context->functions.strcat(tail, va_arg(ap, int) ? truestr : falsestr);
               ret += end - tail;
               break;
            default:
               return -1;
         }
         i++;
      }
      else
      {
         *tail = format[i];
         ret++;
      }
   }

   return ret;
}


int jsnprintf(struct Relocatable* context, char *out, size_t n, const char *format, ...)
{
   va_list ap;
   va_start(ap, format);
   int ret = jvsnprintf(context, out, n, format, ap);
   va_end(ap);
   return ret;
}

char lookup_escape_code(char c)
{
   char popular_chars[][2] = {
      {'\x22', '"'},
      {'\x5c', '\\'},
      {'\x2f', '/'},
      {'\x08', 'b'},
      {'\x0c', 'f'},
      {'\x0a', 'n'},
      {'\x0d', 'r'},
      {'\x09', 't'},
      { '\0' },
   };

   int i = 0;

   for(i = 0; popular_chars[i][0]; i++)
   {
      if(popular_chars[i][0] == c)
         return popular_chars[i][1];
   }

   return '\0';
}

int json_escape(struct Relocatable* context, char *out, char *in, int in_len)
{
   // Escape string accordingly to IETF RFC4627

   int bytes_written = 0;

   context->functions.memset(out, 0, in_len * 6);

   int i;
   for(i = 0; i < in_len; i++)
   {
      char *tail = &out[context->functions.strlen(out)];

      if(lookup_escape_code(in[i]))
      {
         DEFINE_STRING(formatter, "%c%c");
         context->functions.sprintf(tail, formatter, '\\', lookup_escape_code(in[i]));
         bytes_written += 2;
      }
      else if(in[i] & (1 << 7))
      {
         DEFINE_STRING(formatter, "\\u00%02x");
         context->functions.sprintf(tail, formatter, (unsigned char) in[i]);
         bytes_written += 6;
      }
      else
      {
         *tail = in[i];
         bytes_written += 1;
      }
   }

   return bytes_written;
}