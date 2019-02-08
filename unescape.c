/*
 * Copyright (c) 2019 YASUOKA Masahiko <yasuoka@yasuoka.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/types.h>
#include <stdlib.h>

#define IS_XDIGIT(_c) (				\
	(('0' <= (_c) && (_c) <= '9')) ||	\
	(('a' <= (_c) && (_c) <= 'f')) ||	\
	(('A' <= (_c) && (_c) <= 'F')))
#define XDIGIT(_c) (						\
	(('0' <= (_c) && (_c) <= '9'))? (_c) - '0' :		\
	(('a' <= (_c) && (_c) <= 'f'))? (_c) - 'a' + 10 :	\
	(('A' <= (_c) && (_c) <= 'F'))? (_c) - 'A' + 10 : (-1))

const char *unescape(const char *, char *, size_t);

/*
 * Unescape the given JSON string reprentation to UTF-8 string.
 * Reference: RFC 7158
 */
const char *
unescape(const char *in, char *out, size_t outsiz)
{
	int		 i, j, esc = 0;
	uint16_t	 W1, W2;
	uint32_t	 U;

	for (i = 0, j = 0; in[i] != '\0'; i++) {
		if (j >= outsiz)
			goto fail;
		if (!esc) {
			switch (in[i]) {
			case '\\':
				    esc = 1;
				    break;
			default:
				    out[j++] = in[i];
				    break;
			}
		} else {
			esc = 0;
			switch (in[i]) {
			case '"':
			case '\\':
			case '/':
				out[j++] = in[i];
				break;
			default:
				goto fail;
			case 'b':
				out[j++] = 0x08;
				break;
			case 'f':
				out[j++] = 0x0c;
				break;
			case 'n':
				out[j++] = 0x0a;
				break;
			case 'r':
				out[j++] = 0x0d;
				break;
			case 't':
				out[j++] = 0x09;
				break;
			case 'u':
				/*
				 * RFC 2781, 2.2 Decoding UTF-16
				 */
				if (!IS_XDIGIT(in[i + 1]) ||
				    !IS_XDIGIT(in[i + 2]) ||
				    !IS_XDIGIT(in[i + 3]) ||
				    !IS_XDIGIT(in[i + 4]))
					goto fail;
				W1 =
				    (XDIGIT(in[i + 1]) << 12) |
				    (XDIGIT(in[i + 2]) << 8) |
				    (XDIGIT(in[i + 3]) << 4) |
				    XDIGIT(in[i + 4]);
				i += 5;

				if (W1 < 0xD800 || 0xDFFF < W1)
					U = W1;
				else if (!(0xD800 <= W1 && W1 <= 0xDBFF))
					goto fail;
				else {
					if (in[i] != '\\' ||
					    in[i + 1] != 'u' ||
					    !IS_XDIGIT(in[i + 2]) ||
					    !IS_XDIGIT(in[i + 3]) ||
					    !IS_XDIGIT(in[i + 4]) ||
					    !IS_XDIGIT(in[i + 5]))
						goto fail;
					W2 =
					    (XDIGIT(in[i + 2]) << 12) |
					    (XDIGIT(in[i + 3]) << 8) |
					    (XDIGIT(in[i + 4]) << 4) |
					    XDIGIT(in[i + 5]);
					i += 6;

					if (!(0xDC00 <= W2 && W2 <= 0xDFFF))
						goto fail;

					U = ((W1 & 0x3FF) << 10) | (W2 & 0x3FF);
					U += 0x10000;
				}
				/*
				 * RFC 3629, 3. UTF-8 definition
				 */
				if (U <= 0x0000007F)
					out[j++] = U & 0x7F;
				else if (U <= 0x000007FF) {
					if (j + 1 >= outsiz)
						goto fail;
					out[j++] = 0xC0 | ((U >> 6) & 0x1F);
					out[j++] = 0x80 | (U & 0x3F);
				} else if (U <= 0x0000FFFF) {
					if (j + 2 >= outsiz)
						goto fail;
					out[j++] = 0xE0 | ((U >> 12) & 0x0F);
					out[j++] = 0x80 | ((U >> 6) & 0x3F);
					out[j++] = 0x80 | (U & 0x3F);
				} else if (0x00010000 <= U && U <= 0x0010FFFF) {
					if (j + 3 >= outsiz)
						goto fail;
					out[j++] = 0xF0 | ((U >> 18) & 0x07);
					out[j++] = 0x80 | ((U >> 12) & 0x3F);
					out[j++] = 0x80 | ((U >> 6) & 0x3F);
					out[j++] = 0x80 | (U & 0x3F);
				} else
					goto fail;
				--i;	/* prepare for ++ at the for loop */
			}
		}
	}
	if (j >= outsiz)
		goto fail;
	out[j++] = '\0';

	return (out);

 fail:
	return (NULL);
}
