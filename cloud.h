void init_clouds(void);
void run_clouds(void);
int create_cloud(int type);
int simple_cloud(int type, int x, int y, int col, int timeout);
char nearby(int x, int y);
int quick_cloud(int type, int x, int y, int x_speed, int y_speed, int timeout, int colour, int drag);
void double_ball(int x, int y, int size1, int size2, int col);
