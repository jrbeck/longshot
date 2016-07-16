



typedef struct {
  v3d_t pos;
  v3d_t vel;
} phys_state_t;


typedef struct {
  v3d_t dp;
  v3d_t dv;
} phys_derivative_t;







  v3d_t acceleration (size_t i, double t, const phys_state_t &state);
  phys_derivative_t evaluate (size_t i, double t);
  phys_derivative_t evaluate (size_t i, double t, double dt, const phys_derivative_t &d);
  void integrate (size_t i, double t, double dt, octree_c &oct);








// apply the appropriate accelerations
// factor in all forces
v3d_t physics_c::acceleration (size_t i, double t, const phys_state_t &state) {
  v3d_t ret = v3d_zero ();

  ret.y += DEFAULT_GRAVITY;

  // apply forces
  for (size_t f = 0; f < obj[i].force.size(); f++) {
    if (t >= obj[i].force[f].time_start &&
      t < obj[i].force[f].time_end) {
        ret = v3d_add (ret, v3d_scale (obj[i].force[f].vector, obj[i].one_over_mass));
    }
  }

  // apply air resistance
/*  if (abs (state.vel.x) < EPSILON) {
    ret.x = ret.x - (state.vel.x * DEFAULT_AIR_RESISTANCE);
  }
  if (abs (state.vel.y) < EPSILON) {
    ret.y = ret.y - (state.vel.z * DEFAULT_AIR_RESISTANCE);
  }
  if (abs (state.vel.z) < EPSILON) {
    ret.z = ret.z - (state.vel.z * DEFAULT_AIR_RESISTANCE);
  }
*/

  // apply friction if on ground
  if (obj[i].on_ground) {
    v3d_t ground_vel = state.vel;
    ground_vel.y = 0.0;

    double g_v_mag = v3d_mag  (ground_vel);

    if (g_v_mag >= EPSILON) {
      double fric;

      if (g_v_mag < 4.0) {
//        fric = -DEFAULT_FRICTION * (1.0 / abs (g_v_mag));
        fric = -0.75 * ((4.0 - g_v_mag) + (4.0 * 4.0 * DEFAULT_FRICTION));
      }
      else {
        fric = -DEFAULT_FRICTION * (g_v_mag * g_v_mag);
      }

//      printf ("speed (%.3f), fric: %.3f\n", g_v_mag, fric);

      v3d_t drag_vec = v3d_scale (v3d_normalize (ground_vel), fric);

      ret = v3d_add (ret, drag_vec);
    }
  }

  return ret;
}


phys_derivative_t physics_c::evaluate (size_t i, double t) {
  phys_derivative_t output;

  output.dp = obj[i].state.vel;
  output.dv = acceleration (i, t, obj[i].state);

  return output;
}


phys_derivative_t physics_c::evaluate (size_t i, double t, double dt, const phys_derivative_t &d) {
  phys_state_t state;
  state.pos = v3d_add (obj[i].state.pos, v3d_scale (d.dp, dt));
  state.vel = v3d_add (obj[i].state.vel, v3d_scale (d.dv, dt));

  phys_derivative_t output;
  output.dp = state.vel;
  output.dv = acceleration (i, t, state);

  return output;
}


void physics_c::integrate (size_t i, double t, double dt, octree_c &oct) {
  // get the 4 samples
  phys_derivative_t a = evaluate (i, t);
  phys_derivative_t b = evaluate (i, t + (dt * 0.5f), dt * 0.5f, a);
  phys_derivative_t c = evaluate (i, t + (dt * 0.5f), dt * 0.5f, b);
  phys_derivative_t d = evaluate (i, t + dt, dt, c);

  v3d_t dpdt, dvdt;

  // calculate the velocity
  dpdt.x = (1.0 / 6.0) * (a.dp.x + 2.0 * (b.dp.x + c.dp.x) + d.dp.x);
  dpdt.y = (1.0 / 6.0) * (a.dp.y + 2.0 * (b.dp.y + c.dp.y) + d.dp.y);
  dpdt.z = (1.0 / 6.0) * (a.dp.z + 2.0 * (b.dp.z + c.dp.z) + d.dp.z);

  // calculate the acceleration
  dvdt.x = (1.0 / 6.0) * (a.dv.x + 2.0 * (b.dv.x + c.dv.x) + d.dv.x);
  dvdt.y = (1.0 / 6.0) * (a.dv.y + 2.0 * (b.dv.y + c.dv.y) + d.dv.y);
  dvdt.z = (1.0 / 6.0) * (a.dv.z + 2.0 * (b.dv.z + c.dv.z) + d.dv.z);

  v3d_t disp = v3d_scale (dpdt, dt);

  //old method: uses vector against blocks
//  clip_displacement_against_blocks (oct, obj[i].state.pos, disp);

  // clip against the world using bbs
  clip_displacement_against_world (oct, i, disp);

  // check for collisions with other objects, either apply physics or pick up...
  //check_for_collision (i);
  //check_for_collision (int index);


  // integrate to the new state
  obj[i].state.pos = v3d_add (obj[i].state.pos, disp);
  obj[i].state.vel = v3d_add (obj[i].state.vel, v3d_scale (dvdt, dt));

  // update the BoundingBox position
  obj[i].boundingBox.setPosition (obj[i].state.pos);

  // if the displacement clip clipped anything, fix the velocity
  if (abs (disp.x) < EPSILON) obj[i].state.vel.x = 0.0;
  if (abs (disp.y) < EPSILON) obj[i].state.vel.y = 0.0;
  if (abs (disp.z) < EPSILON) obj[i].state.vel.z = 0.0;
}
