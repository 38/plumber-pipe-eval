#include <pservlet.h>
#include <pstd.h>
#include <stdlib.h>
#include  <string.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
typedef struct {
	pipe_t in, out;
	uint32_t size;
	uint64_t _ts[128];
	uint32_t _tsc;
	const char* name;
} ctx_t;

static inline void record_time(ctx_t* ctx)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ctx->_ts[ctx->_tsc] = ts.tv_sec * 1000000000ull;
	ctx->_ts[ctx->_tsc] += ts.tv_nsec;
	ctx->_tsc ++;
}
static int _init(uint32_t argc, char const* const* argv, void* ctxmem)
{
	ctx_t* ctx = (ctx_t*)ctxmem;
	ctx->in = pipe_define("in", PIPE_INPUT, NULL);
	ctx->out = pipe_define("out", PIPE_OUTPUT, NULL);
	ctx->size = atoi(argv[2]);
	ctx->name = argv[1];
	ctx->_tsc = 0;
	return 0;
}
static int _exec(void* ctxmem)
{
	ctx_t* ctx = (ctx_t*)ctxmem;

	char buf[ctx->size];
	uint32_t todo = ctx->size;
	while(todo > 0) todo -= pipe_read(ctx->in, buf + ctx->size - todo, todo);
	record_time(ctx);
	todo = ctx->size;
	record_time(ctx);
	while(todo > 0) todo -= pipe_write(ctx->out, buf + ctx->size - todo, todo);

	return 0;
}

static int _cleanup(void* ctxmem)
{
	ctx_t* ctx = (ctx_t*)ctxmem;
	uint32_t i;
	for(i = 0; i < ctx->_tsc; i ++)
		printf("%s[%u] = %"PRIu64"\n", ctx->name, i, ctx->_ts[i]);
	return 0;
}

SERVLET_DEF = {
	.desc = "source servlet",
	.version = 0x0,
	.size =sizeof(ctx_t),
	.init = _init,
	.unload = _cleanup,
	.exec = _exec
};
