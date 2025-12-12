#include "../common.h"

da_struct(char*, dest_t)

typedef struct {
	char* in;
	uint64_t incoming;
	dest_t out;
} conn_t;

da_struct(conn_t, input_t)

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	fseek(f_in, 0, SEEK_END);
	uint64_t len = ftell(f_in);
	fseek(f_in, 0, SEEK_SET);

	char* s = calloc(len + 1, sizeof(char));
	fread(s, len, 1, f_in);

	da_init(out);
	char* in, *outc;
	dest_t dest = {0};
	conn_t conn = {0};

	char* line_sptr, *conn_sptr;
	char* line = strtok_r(s, "\n", &line_sptr);
	while (line != NULL) {
		da_init(&(dest));
		in = strdup(strtok_r(line, ": ", &conn_sptr));
		while ((outc = strtok_r(NULL, " ", &conn_sptr)) != NULL) {
			da_append(&(dest), strdup(outc));
		}
		conn.in = in;
		conn.out = dest;
		da_append(out, conn);
		line = strtok_r(NULL, "\n", &line_sptr);
	}

	fclose(f_in);
	free(s);
	return 0;
}

// n1 and n2 are assumed to be 3 characters long
bool cmp_names(char* n1, char* n2) {
	return n1[0] == n2[0] && n1[1] == n2[1] && n1[2] == n2[2];
}

typedef struct edge_t {
	uint32_t to;
	struct edge_t* next;
} edge_t;

typedef struct {
	uint32_t v; // amount of vertices
	edge_t** adj;
} graph_t;

graph_t graph_new(uint32_t v) {
	graph_t graph = {0};
	graph.v = v;
	graph.adj = calloc(v, sizeof(edge_t*));
	return graph;
}

void add_edge(graph_t* g, uint32_t from, uint32_t to) {
	edge_t* e = malloc(sizeof(edge_t));
	e->to = to;
	e->next = g->adj[from];
	g->adj[from] = e;
}

uint32_t find_idx(input_t* input, char* n) {
	for (uint32_t i = 0; i < input->len; i++) {
		if (cmp_names(n, input->items[i].in)) return i;
	}
	return -1;
}

void print_graph(const graph_t* g) {
	for (int v = 0; v < g->v; v++) {
		printf("%d:", v);
		edge_t* e = g->adj[v];
		while (e != NULL) {
			printf(" %d", e->to);
			e = e->next;
		}
		printf("\n");
	}
}

// idx is the node we start from
uint64_t dfs(input_t* input, graph_t* g, uint32_t idx, char* target) {
	if (cmp_names(input->items[idx].in, target)) return 1;
	uint64_t res = 0;
	edge_t* outgoing = g->adj[idx];
	while (outgoing != NULL) {
		res += dfs(input, g, outgoing->to, target);
		outgoing = outgoing->next;
	}
	return res;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	if (find_idx(input, "you") == -1) {
		printf("this is part 2's example! (or just bad file)\n");
		return;
	}
	// add dummy node out
	char* out_name = strdup("out");
	dest_t dest = {0};
	da_append(input, ((conn_t){out_name, 0, dest}));

	graph_t graph = graph_new(input->len);
	for (uint32_t i = 0; i < input->len; i++) {
		for (uint32_t j = 0; j < input->items[i].out.len; j++) {
			add_edge(&graph, find_idx(input, input->items[i].in),
				find_idx(input, input->items[i].out.items[j]));
		}
	}

	res = dfs(input, &graph, find_idx(input, "you"), "out");
	printf("part 1 answer: %lu\n", res);
	return;
}

#define NOPE 0xFFFFFFFFFFFFFFFF
uint64_t cache[3000];

uint64_t dfs2(input_t* input, graph_t* g, uint32_t idx,
	bool has_fft, bool has_dac) {
	if (cache[(idx<<2) | ((has_fft ? 1 : 0) << 1) | (has_dac ? 1 : 0)] != NOPE)
		return cache[(idx<<2) | ((has_fft ? 1 : 0) << 1) | (has_dac ? 1 : 0)];
	if (cmp_names(input->items[idx].in, "out")) {
		cache[(idx<<2) | ((has_fft ? 1 : 0) << 1) | (has_dac ? 1 : 0)] = (has_fft && has_dac) ? 1 : 0;
		return (has_fft && has_dac) ? 1 : 0;
	}
	uint64_t res = 0;
	edge_t* outgoing = g->adj[idx];
	while (outgoing != NULL) {
		if (outgoing->to == find_idx(input, "fft")) {
			res += dfs2(input, g, outgoing->to, true, has_dac);
		} else if (outgoing->to == find_idx(input, "dac")) {
			res += dfs2(input, g, outgoing->to, has_fft, true);
		} else {
			res += dfs2(input, g, outgoing->to, has_fft, has_dac);
		}
		outgoing = outgoing->next;
	}
	//printf("setting cache %u to %lu", (idx<<2) | ((has_fft ? 1 : 0) << 1) | (has_dac ? 1 : 0), res);
	cache[(idx<<2) | ((has_fft ? 1 : 0) << 1) | (has_dac ? 1 : 0)] = res;
	return res;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	// add dummy node out
	char* out_name = strdup("out");
	dest_t dest = {0};
	da_append(input, ((conn_t){out_name, 0, dest}));

	graph_t graph = graph_new(input->len);
	for (uint32_t i = 0; i < input->len; i++) {
		for (uint32_t j = 0; j < input->items[i].out.len; j++) {
			add_edge(&graph, find_idx(input, input->items[i].in),
				find_idx(input, input->items[i].out.items[j]));
		}
	}

	for (uint32_t i = 0; i < 3000; i++) cache[i] = NOPE;

	res = dfs2(input, &graph, find_idx(input, "svr"), false, false);
	printf("part 2 answer: %lu\n", res);
	return;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("provide an input file name\n");
		return 1;
	}

	input_t input = {0};
	if (gather_input(argv[1], &input)) {
		printf("something has gone horribly wrong!\n");
		return 1;
	}

	double time;

	time = time_function(part1, &input);
	printf("took %.10lf seconds to run part 1\n", time);

	time = time_function(part2, &input);
	printf("took %.10lf seconds to run part 2\n", time);

	for (uint32_t i = 0; i < input.len; i++) {
		for (uint32_t j = 0; j < input.items[i].out.len; j++) {
			free(input.items[i].out.items[j]);
		}
		da_free(&(input.items[i].out));
		free(input.items[i].in);
	}
	da_free(&(input));

	return 0;
}