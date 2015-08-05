#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

unsigned int getData(const unsigned char *p, int len) {
	unsigned int res = 0;
	int i;
	for (i = len - 1; i >= 0; i--) {
		res = (res << 8) | p[i];
	}
	return res;
}

void setData(unsigned char *p, unsigned int data, int len) {
	int i;
	for (i = 0; i < len; i++) {
		p[i] = data & 0xff;
		data >>= 8;
	}
}

int isMatch(const unsigned char *data, const char *expected, int len) {
	int i;
	for (i = 0; i < len; i++) {
		if (data[i] != ((const unsigned char*)expected)[i]) return 0;
	}
	return 1;
}

int main(int argc, char *argv[]) {
	FILE *fp;
	unsigned char header[44] = {};
	unsigned int channel_num;
	unsigned int sample_rate;
	unsigned int bit_per_sample;
	unsigned int block_size;
	unsigned int data_size;
	unsigned int sample_num;
	unsigned int sample;
	unsigned int channel;
	int i;
	int retnum;
	lua_State *s;
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <input file> <output file> <script file> [parameters...]\n", argc > 0 ? argv[0] : "wav_lua");
		return 1;
	}
	if (argv[1][0] != '-' || argv[1][1] != '\0') {
		unsigned int header_size;
		fp = fopen(argv[1], "rb");
		if (fp == NULL) {
			fputs("input file open error\n", stderr);
			return 1;
		}
		if (fread(header, 1, 44, fp) != 44) {
			fputs("input file read error\n", stderr);
			fclose(fp);
			return 1;
		}
		channel_num = getData(&header[22], 2);
		sample_rate = getData(&header[24], 4);
		bit_per_sample = getData(&header[34], 2);
		block_size = getData(&header[32], 2);

		header_size = getData(&header[16], 4);
		if (header_size > 16) {
			if (fseek(fp, 20 + header_size, SEEK_SET)) {
				fputs("input file seek error\n", stderr);
				fclose(fp);
				return 1;
			}
			header[16] = 0x10; header[17] = 0; header[18] = 0; header[19] = 0;
			for (;;) {
				if (fread(&header[36], 1, 8, fp) != 8) {
					fputs("input file read error\n", stderr);
					fclose(fp);
					return 1;
				}
				if (isMatch(&header[36], "data", 4)) {
					break;
				} else {
					unsigned int chunk_size = getData(&header[40], 4);
					if (fseek(fp, chunk_size, SEEK_CUR)) {
						fputs("input file seek error\n", stderr);
						fclose(fp);
						return 1;
					}
				}
			}
		}

		data_size = getData(&header[40], 4);
		if (!isMatch(&header[0], "RIFF", 4) || !isMatch(&header[8], "WAVEfmt ", 8) || !isMatch(&header[36], "data", 4) ||
		getData(&header[20], 2) != 1 || getData(&header[28], 4) != channel_num * bit_per_sample / 8 * sample_rate ||
		block_size != channel_num * bit_per_sample / 8 || data_size % block_size != 0 ||
		(bit_per_sample != 8 && bit_per_sample != 16)) {
			fputs("unsupported input format\n", stderr);
			return 1;
		}
		sample_num = data_size / block_size;
		s = luaL_newstate();
		if (s == NULL) {
			fputs("failed to create Lua state\n", stderr);
			fclose(fp);
			return 1;
		}
		luaL_openlibs(s);
		if (luaL_dofile(s, argv[3])) {
			const char *str = luaL_checkstring(s, -1);
			fputs("failed to load Lua script\n", stderr);
			if (str != NULL) fputs(str, stderr);
			fclose(fp);
			lua_close(s);
			return 1;
		}

		if (lua_getglobal(s, "process") != LUA_TFUNCTION) {
			fputs("process is not a function\n", stderr);
			fclose(fp);
			lua_close(s);
			return 1;
		}

		lua_createtable(s, channel_num, 0);
		for (channel = 0; channel < channel_num; channel++) {
			lua_createtable(s, sample_num, 0);
		}
		for (sample = 1; sample <= sample_num; sample++) {
			for (channel = 0; channel < channel_num; channel++) {
				unsigned char data[2];
				double value = 0;
				if (fread(data, 1, bit_per_sample / 8, fp) != bit_per_sample / 8) {
					fputs("input read error\n", stderr);
					fclose(fp);
					lua_close(s);
					return 1;
				}
				if (bit_per_sample == 8) {
					value = (double)(data[0] - 128) / 128.0;
				} else if (bit_per_sample == 16) {
					unsigned int v = getData(data, 2);
					if (v & 0x8000) {
						value = (double)(-((int)(~v+1) & 0xffff)) / 32768.0;
					} else {
						value = (double)v / 32768.0;
					}
				}
				lua_pushnumber(s, value);
				lua_seti(s, -2 - channel, sample);
			}
		}
		fclose(fp);
		for (channel = 1; channel <= channel_num; channel++) {
			lua_seti(s, 2, channel);
		}
	} else {
		header[0] = 'R'; header[1] = 'I'; header[2] = 'F'; header[3] = 'F';
		header[8] = 'W'; header[9] = 'A'; header[10] = 'V'; header[11] = 'E';
		header[12] = 'f'; header[13] = 'm'; header[14] = 't'; header[15] = ' ';
		header[16] = 0x10; header[17] = 0; header[18] = 0; header[19] = 0;
		header[20] = 1; header[21] = 0;
		header[36]= 'd'; header[37] = 'a'; header[38] = 't'; header[39] = 'a';

		channel_num = 0;
		sample_rate = 44100;
		bit_per_sample = 16;
		block_size = 0;
		data_size = 0;

		s = luaL_newstate();
		if (s == NULL) {
			fputs("failed to create Lua state\n", stderr);
			return 1;
		}
		luaL_openlibs(s);
		if (luaL_dofile(s, argv[3])) {
			const char *str = luaL_checkstring(s, -1);
			fputs("failed to load Lua script\n", stderr);
			if (str != NULL) fputs(str, stderr);
			lua_close(s);
			return 1;
		}

		if (lua_getglobal(s, "process") != LUA_TFUNCTION) {
			fputs("process is not a function\n", stderr);
			lua_close(s);
			return 1;
		}

		lua_createtable(s, channel_num, 0);
	}

	lua_pushinteger(s, sample_rate);

	lua_createtable(s, argc - 4, 0);
	for (i = 0; i < argc - 4; i++) {
		lua_pushstring(s, argv[i + 4]);
		lua_seti(s, -2, i + 1);
	}

	if (lua_pcall(s, 3, LUA_MULTRET, 0) != LUA_OK) {
		const char *str = luaL_checkstring(s, -1);
		fputs("error in executing Lua script\n", stderr);
		if (str != NULL) fputs(str, stderr);
		lua_close(s);
		return 1;
	}

	if (argv[2][0] != '-' || argv[2][1] != '\0') {
		retnum = lua_gettop(s);
		if (retnum > 3) {
			fputs("too many return data from Lua script\n", stderr);
			lua_close(s);
			return 1;
		} else if (retnum > 0 && !lua_isnil(s, 1)) {
			if (!lua_istable(s, 1)) {
				fputs("the first return data is neither a table nor nil\n", stderr);
				lua_close(s);
				return 1;
			}
			if (retnum >= 2) {
				if (lua_isinteger(s, 2)) {
					sample_rate = luaL_checkinteger(s, 2);
				} else {
					fputs("the second return data is not an integer\n", stderr);
					lua_close(s);
					return 1;
				}
			}
			if (retnum >= 3) {
				if (lua_isinteger(s, 3)) {
					bit_per_sample = luaL_checkinteger(s, 3);
				} else {
					fputs("the third return data is not an integer\n", stderr);
					lua_close(s);
					return 1;
				}
			}
		}
		if (retnum > 1) lua_pop(s, retnum - 1);
		channel_num = luaL_len(s, 1);
		if (channel_num > 0) {
			if (bit_per_sample != 8 && bit_per_sample != 16) {
				fputs("unsupported output bit per sample\n", stderr);
				lua_close(s);
				return 1;
			}
			for (channel = 1; channel <= channel_num; channel++) {
				lua_geti(s, 1, channel);
				if (channel == 1) {
					sample_num = luaL_len(s, -1);
				} else if (sample_num != luaL_len(s, -1)) {
					fputs("the number of samples mismatch\n", stderr);
					lua_close(s);
					return 1;
				}
			}
			setData(&header[4], channel_num * bit_per_sample / 8 * sample_num + 36, 4);
			setData(&header[22], channel_num, 2);
			setData(&header[24], sample_rate, 4);
			setData(&header[28], channel_num * bit_per_sample / 8 * sample_rate, 4);
			setData(&header[32], channel_num * bit_per_sample / 8, 2);
			setData(&header[34], bit_per_sample, 2);
			setData(&header[40], channel_num * bit_per_sample / 8 * sample_num, 4);
			for (sample = 1; sample <= sample_num; sample++) {
				for (channel = 1; channel <= channel_num; channel++) {
					lua_geti(s, channel + 1, sample);
					if (!lua_isnumber(s, -1)) {
						fputs("some of the data is invalid\n", stderr);
						lua_close(s);
						return 1;
					}
					lua_pop(s, 1);
				}
			}
			fp = fopen(argv[2], "wb");
			if (fp == NULL) {
				fputs("output file open error\n", stderr);
				lua_close(s);
				return 1;
			}
			if (fwrite(header, 1, 44, fp) != 44) {
				fputs("output file write error\n", stderr);
				fclose(fp);
				lua_close(s);
				return 1;
			}
			for (sample = 1; sample <= sample_num; sample++) {
				for (channel = 1; channel <= channel_num; channel++) {
					double value;
					unsigned char data[2];
					int v;
					lua_geti(s, channel + 1, sample);
					value = luaL_checknumber(s, -1);
					if (value < -1.0) value = -1.0;
					if (value > 1.0) value = 1.0;
					lua_pop(s, 1);
					if (bit_per_sample == 8) {
						v = (int)(value * 128 + 128);
						if (v < 0) v = 0;
						if (v > 255) v = 255;
						data[0] = (unsigned char)v;
						if (fwrite(data, 1, 1, fp) != 1) {
							fputs("output file write error\n", stderr);
							fclose(fp);
							lua_close(s);
							return 1;
						}
					} else if (bit_per_sample == 16) {
						v = (int)(value * 32768);
						if (v < -32768) v = -32768;
						if (v > 32767) v = 32767;
						setData(data, v, 2);
						if (fwrite(data, 1, 2, fp) != 2) {
							fputs("output file write error\n", stderr);
							fclose(fp);
							lua_close(s);
							return 1;
						}
					}
				}
			}
			fclose(fp);
		}

		lua_pop(s, channel_num + 1);
	} else {
		lua_pop(s, lua_gettop(s));
	}
	lua_close(s);
	return 0;
}
