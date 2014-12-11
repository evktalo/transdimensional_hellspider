
void run_hs(void);

void hurt_part(int p, int dam);
void hurt_core(int dam);
void apply_torque_to_hs(int h, int x, int y, int force, int angle);
void hs_impulse(int h, int force_angle, int impulse);
void set_drive_directions(int h);
void calculate_moment_of_inertia(int h);
void calculate_centre_of_mass(int h);
void run_part(int h, int p);
void hs_change_position(int h, int change);
