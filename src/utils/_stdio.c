#include <stdarg.h>
#include <unistd.h>

#define CONVERSION_TRIGGER '%'
#define MINUS_SIGN '-'
#define DIGITS "0123456789abcdef"
#define OCTAL 8
#define DECIMAL 10
#define HEXADECIMAL 16
#define HEXA_PREFIX "0x"
#define NULL_STRING_PLACEHOLDER "(null)"
#define FLUSH_TRIGGER '\n'
#define BUFSIZ 1024

typedef struct s_output_buffer
{
    unsigned short index;
    unsigned int count;
    char array[BUFSIZ];
    int fd;
} output_buffer;

static int _vdprintf(int fd, const char* restrict format, va_list* args);
static const char* handle_format_char(const char* restrict format, va_list* args, output_buffer* buffer);
static void handle_conversion(char specifier, va_list* args, output_buffer* buffer);
static void c_specifier(char c, output_buffer* buffer);
static void d_specifier(int d, output_buffer* buffer);
static void o_specifier(unsigned int u, output_buffer* buffer);
static void p_specifier(void* p, output_buffer* buffer);
static void s_specifier(char* s, output_buffer* buffer);
static void u_specifier(unsigned int u, output_buffer* buffer);
static void x_specifier(unsigned int u, output_buffer* buffer);
static void print_unsigned_long(unsigned long u, unsigned char base, output_buffer* buffer);
static void print_non_zero_unsigned_long(unsigned long u, unsigned char base, output_buffer* buffer);
static void print_string(char* s, output_buffer* buffer);
static void print_char(char c, output_buffer* buffer);
static void put(char c, output_buffer* buffer);
static void flush_if_full(output_buffer* buffer);
static void flush(output_buffer* buffer);

int _dprintf(int fd, const char* restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int result = _vdprintf(fd, format, &args);
    va_end(args);
    return result;
}

int _printf(const char* restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int result = _vdprintf(STDOUT_FILENO, format, &args);
    va_end(args);
    return result;
}

int _puts(const char* string)
{
    return _printf("%s\n", string);
}

static int _vdprintf(int fd, const char* restrict format, va_list* args)
{
    output_buffer buffer = {
            .index = 0,
            .count = 0,
            .fd = fd,
    };
    for (; *format; format++)
    {
        format = handle_format_char(format, args, &buffer);
    }
    flush(&buffer);
    return buffer.count;
}

const char* handle_format_char(const char* restrict format, va_list* args, output_buffer* buffer)
{
    if (*format == CONVERSION_TRIGGER)
    {
        handle_conversion(*(++format), args, buffer);
    }
    else
    {
        print_char(*format, buffer);
    }
    return format;
}

void handle_conversion(char specifier, va_list* args, output_buffer* buffer)
{
    switch(specifier)
    {
        case 'd':
            d_specifier(va_arg(*args, int), buffer);
            return;
        case 'o':
            o_specifier(va_arg(*args, unsigned int), buffer);
            return;
        case 'u':
            u_specifier(va_arg(*args, unsigned int), buffer);
            return;
        case 'x':
            x_specifier(va_arg(*args, unsigned int), buffer);
            return;
        case 'c':
            c_specifier((char) va_arg(*args, int), buffer);
            return;
        case 's':
            s_specifier(va_arg(*args, char*), buffer);
            return;
        case 'p':
            p_specifier(va_arg(*args, void*), buffer);
            return;
        case CONVERSION_TRIGGER:
            print_char(CONVERSION_TRIGGER, buffer);
            return;
    }
}

void c_specifier(char c, output_buffer* buffer)
{
    print_char(c, buffer);
}

void d_specifier(int d, output_buffer* buffer)
{
    if (d < 0)
    {
        print_char(MINUS_SIGN, buffer);
        d = -d;
    }
    print_unsigned_long(d, DECIMAL, buffer);
}

void o_specifier(unsigned int u, output_buffer* buffer)
{
    print_unsigned_long(u, OCTAL, buffer);
}

void p_specifier(void* p, output_buffer* buffer)
{
    print_string(HEXA_PREFIX, buffer);
    print_unsigned_long((unsigned long) p, HEXADECIMAL, buffer);
}

void s_specifier(char* s, output_buffer* buffer)
{
    print_string(s, buffer);
}

void u_specifier(unsigned int u, output_buffer* buffer)
{
    print_unsigned_long(u, DECIMAL, buffer);
}

void x_specifier(unsigned int u, output_buffer* buffer)
{
    print_unsigned_long(u, HEXADECIMAL, buffer);
}

void print_unsigned_long(unsigned long u, unsigned char base, output_buffer* buffer)
{
    if (u == 0)
    {
        print_char('0', buffer);
        return;
    }
    print_non_zero_unsigned_long(u, base, buffer);
}

void print_non_zero_unsigned_long(unsigned long u, unsigned char base, output_buffer* buffer)
{
    if (u == 0)
    {
        return;
    }
    print_non_zero_unsigned_long(u / base, base, buffer);
    print_char(DIGITS[u % base], buffer);
}

void print_string(char* s, output_buffer* buffer)
{
    if (!s)
    {
        print_string(NULL_STRING_PLACEHOLDER, buffer);
        return;
    }
    while(*s)
    {
        print_char(*s++, buffer);
    }
}

void print_char(char c, output_buffer* buffer)
{
    put(c, buffer);
    buffer->count++;
}

void put(char c, output_buffer* buffer)
{
    buffer->array[buffer->index++] = c;
    if (c == FLUSH_TRIGGER)
    {
        flush(buffer);
        return;
    }
    flush_if_full(buffer);
}

void flush_if_full(output_buffer* buffer)
{
    if (buffer->index == BUFSIZ)
    {
        flush(buffer);
    }
}

void flush(output_buffer* buffer)
{
    write(buffer->fd, buffer->array, buffer->index);
    buffer->index = 0;
}
