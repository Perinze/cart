#include <iostream>
using namespace std;

typedef int ball_t;

#define BALL_RED 1
#define BALL_GREEN 2
#define BALL_BLUE 3
#define BALL_OTHER 4
#define BALL_NONE 5

typedef int repo_t;

#define REPO_RED 1
#define REPO_GREEN 2
#define REPO_BLUE 3

typedef unsigned state_t;

#define CART_STATE_FIND_BALL 1
#define CART_STATE_TOWARD_BALL 2
#define CART_STATE_BRING_BALL 3
#define CART_STATE_FIND_REPO 4
#define CART_STATE_TOWARD_REPO 5
#define CART_STATE_LEAVE_REPO 6
#define CART_STATE_BACK 7

int vis[6 + 4]; // 0 for not visited, 1 red, 2 green, 3 yellow
int bpos[3 + 4]; // index 1-3, value 1-6

inline int i2p(int bp); 
inline int p2i(int p);

inline bool is_target_ball(ball_t ball) { return ball >= 1 && ball <= 3; }

inline int b2p(ball_t ball);

ball_t recognize_ball() {
	turn_left();
	ball = capture();
	if (!is_target_ball(ball)) turn_right();
	return ball;
}

bool on_t_turn();
bool on_left_turn();
bool on_right_turn();
int is_target_repo(repo_t cur, repo_t tar) { return tar - cur; }

int total_ball;
int detect_index; // 1-6
int cur_pos; // 1-9
ball_t cur_ball;
int cur_ball_pos; // 1-9
int tar_ball_pos; // 1-9
int tar_repo_pos; // 1-9

bool back_to_branch() {
	return on_t_turn() || (cur_pos == 9 && on_right_turn());
}

void init() {
	// total_ball = 0;
	// cur_pos = 0;
	direct = FORWARD;
	detect_index = 1;
	tar_ball_pos = i2p(detect_index);
}
void state_update() {
	switch (state) {
		case CART_STATE_FIND_BALL:
			int dpos = is_target_ball_pool(cur_pos, tar_ball_pos);
			if (!same_dir(dpos, direct)) {
				turn_back();
				change_direct(&direct);
			}
			if (dpos > 0) {
				forward();
			} else {
				int side = get_turn(direct, TO_LEFT);
				turn(side);
				cur_ball = recognize_ball();
				if (is_target_ball(cur_ball)) {
					cur_ball_pos = i2p(detect_index);
					tar_repo_pos = b2p(cur_ball);
					state = CART_STATE_TOWARD_BALL;
				} else {
					turn(opposite(side));
				}
			}
			break;
		case CART_STATE_TOWARD_BALL:
			state = (having_ball() ? CART_STATE_BRING_BALL : state);
			break;
		case CART_STATE_BRING_BALL:
			if (back_to_branch()) {
				int dpos = tar_repo_pos - cur_pos;
				int side = get_turn(dpos, FROM_LEFT);
				turn(side);
				direct = get_direct(side, FROM_LEFT);
				// dpos = (dpos > 0 : dpos : -dpos);
				state = CART_STATE_FIND_REPO;
			}
			break;
		case CART_STATE_FIND_REPO:
			// forward until tar repo (for each step, forward until a turn)
			int dpos = is_target_repo(cur_pos, tar_repo_pos);
			if (!same_dir(dpos, direct)) turn_back();
			if (dpos > 0) {
				forward();
			} else if (dpos == 0) {
				// stop();
				int side = get_turn(direct, TO_RIGHT);
				turn(side);
				state = CART_STATE_TOWARD_REPO;
			}
			break;
		case CART_STATE_TOWARD_REPO:
			state = (!having_ball() ? CART_STATE_LEAVE_REPO	: state);
			break;
		case CART_STATE_LEAVE_REPO:
			if (back_to_branch()) {
				if (total_ball == 3) {
					turn_left();
					state = CART_STATE_BACK;
				} else {
					tar_ball_pos = i2p(detect_index+1);
					int dpos = tar_ball_pos - cur_pos;
					int side = get_turn(dpos, FROM_RIGHT);
					turn(side);
					direct = get_direct(side, FROM_RIGHT);
					state = CART_STATE_FIND_BALL;
				}
			}
			break;
	}
}
