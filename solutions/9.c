#include "../common.h"

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

typedef struct {
	int64_t x, y;
} tile_t;

da_struct(tile_t, input_t)

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(out);

	int64_t num = 0; tile_t tile = {0};
	while (fscanf(f_in, "%ld", &num) == 1) {
		tile.x = num;
		fgetc(f_in); // comma
		fscanf(f_in, "%ld", &num);
		tile.y = num;
		da_append(out, tile);
	}

	fclose(f_in);
	return 0;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	int64_t area = 0;
	for (uint32_t i = 0; i < input->len; i++) {
		for (uint32_t j = 0; j < input->len; j++) {
			area = (labs(input->items[i].x - input->items[j].x) + 1) *
					(labs(input->items[i].y - input->items[j].y) + 1);
			if (area > res) res = area;
		}
	}

	printf("part 1 answer: %lu\n", res);
	return;
}

typedef struct {
	tile_t p1, p2;
} edge_t;

da_struct(edge_t, edges_t)

bool point_in_polygon(tile_t p, edges_t h_edges, edges_t v_edges) {
	int64_t px = p.x, py = p.y;
	bool inside = false;

	for (uint32_t i = 0; i < h_edges.len; i++) {
		if (p.y == h_edges.items[i].p1.y &&
			p.x >= MIN(h_edges.items[i].p1.x, h_edges.items[i].p2.x) &&
			p.x <= MAX(h_edges.items[i].p1.x, h_edges.items[i].p2.x)) {
			return true; // point on h_edge
		}
	}

	for (uint32_t i = 0; i < v_edges.len; i++) {
		if (p.x == v_edges.items[i].p1.x &&
			p.y >= MIN(v_edges.items[i].p1.y, v_edges.items[i].p2.y) &&
			p.y <= MAX(v_edges.items[i].p1.y, v_edges.items[i].p2.y)) {
			return true; // point on v_edge
		}
	}

	for (uint32_t i = 0; i < v_edges.len; i++) {
		tile_t p1 = v_edges.items[i].p1;
		tile_t p2 = v_edges.items[i].p2;

		if (p1.y > p2.y) {
			tile_t tmp = p1;
			p1 = p2;
			p2 = tmp;
		}

		if (p1.y <= py && py < p2.y) {
			if (p1.x >= px) { inside = !inside; }
		}
	}
	return inside;
}

bool edges_cross(edge_t v, edge_t h) {
	return (v.p1.x > MIN(h.p1.x, h.p2.x) &&
		v.p1.x < MAX(h.p1.x, h.p2.x) &&
		h.p1.y > MIN(v.p1.y, v.p2.y) &&
		h.p1.y < MAX(v.p1.y, v.p2.y));
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	edges_t h_edges = {0}; da_init(&(h_edges));
	edges_t v_edges = {0}; da_init(&(v_edges));

	for (uint32_t i = 0; i < input->len; i++) {
		tile_t p1 = input->items[i];
		tile_t p2 = input->items[(i + 1) % input->len];
		edge_t e = (edge_t){p1, p2};
		if (p1.x == p2.x) {da_append(&(v_edges), e); }
		else { da_append(&(h_edges), e); }
	}

	int64_t ma = 0;
	for (uint32_t i = 0; i < input->len; i++) {
		for (uint32_t j = i + 1; j < input->len; j++) {
			int64_t left = MIN(input->items[i].x, input->items[j].x);
			int64_t right = MAX(input->items[i].x, input->items[j].x);
			int64_t bottom = MAX(input->items[i].y, input->items[j].y);
			int64_t top = MIN(input->items[i].y, input->items[j].y);

			bool flag = false;

			// check if any of the polygon's points are in the rectangle
			for (uint32_t i2 = 0; i2 < input->len; i2++) {
				if (input->items[i2].x > left && input->items[i2].x < right &&
					input->items[i2].y > top && input->items[i2].y < bottom) {
					flag = true;
					break;
				}
			}

			// check if any of the rectangle's points are
			// outside of the polygon
			tile_t topleft = (tile_t){left, top};
			tile_t topright = (tile_t){right, top};
			tile_t bottomleft = (tile_t){left, bottom};
			tile_t bottomright = (tile_t){right, bottom};

			if (!point_in_polygon(topleft, h_edges, v_edges) ||
				!point_in_polygon(topright, h_edges, v_edges) ||
				!point_in_polygon(bottomleft, h_edges, v_edges) ||
				!point_in_polygon(bottomright, h_edges, v_edges)) {
				flag = true;
			}

			// check if any of the rectangles edges cross
			// any of the polygon's edges
			edge_t te = (edge_t){topleft, topright};
			edge_t be = (edge_t){bottomleft, bottomright};
			edge_t re = (edge_t){topright, bottomright};
			edge_t le = (edge_t){topleft, bottomleft};
			for (uint32_t i = 0; i < h_edges.len; i++) {
				if (edges_cross(re, h_edges.items[i]) ||
					edges_cross(le, h_edges.items[i])) {
					flag = true; break;
				}
			}
			for (uint32_t i = 0; i < v_edges.len; i++) {
				if (edges_cross(te, v_edges.items[i]) ||
					edges_cross(be, v_edges.items[i])) {
					flag = true; break;
				}
			}

			if (flag) continue;
			int64_t area = (labs(right - left) + 1) * (labs(top - bottom) + 1);
			ma = MAX(ma, area);
		}
	}

	res = ma;
	printf("part 2 answer: %lu\n", res);
	da_free(&(h_edges));
	da_free(&(v_edges));
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

	da_free(&(input));

	return 0;
}