#ifndef _InputEventQueue_h
#define _InputEventQueue_h

#include <vector>
#include <algorithm>


typedef struct {
  int owner;

  double time;
  int type;
  double value;
} input_event_t;


class InputEventQueue {
public:
  InputEventQueue ();
  ~InputEventQueue ();

  // clear the queue
  int clear (void);

  // add a new event to the queue
  int add_event (double time, int type, double value);

  // remove the soonest event from the list and give it back
  int extract_soonest (input_event_t *e);

  // check to see when the next event occurs
  double time_next (void);

  // helper function for sort ()
  static bool compare_time (input_event_t a, input_event_t b) { return a.time > b.time; }

  // various input event types
  static enum {
    FORWARD_DOWN,      // move forward-back
    FORWARD_UP,
    BACKWARD_DOWN,      // move left-right
    BACKWARD_UP,
    LEFT_DOWN,      // move forward-back
    LEFT_UP,
    RIGHT_DOWN,      // move left-right
    RIGHT_UP,
    CHANGE_FACING,
    CHANGE_ELEV,
    JUMP_DOWN,
    JUMP_UP,
    SHOOT_DOWN,
    SHOOT_UP,
    ROCKET_DOWN,
    ROCKET_UP
  };

private:
  vector<input_event_t> q;    // queue
  bool sorted;          // are the elements sorted?
};




InputEventQueue::InputEventQueue () {
  sorted = false;
}


InputEventQueue::~InputEventQueue () {
  clear ();
}


// clear the queue
int InputEventQueue::clear (void) {
  q.clear ();

  return 0;
}


int InputEventQueue::add_event (double time, int type, double value) {
  input_event_t e;

  e.time = time;
  e.type = type;
  e.value = value;

//  printf ("event: time:%f, type:%d, val:%f\n", time, type, value);

  // push the new event onto the queue
  q.push_back (e);

  // haven't sorted, so just make note
  sorted = false;

  return 0;
}


// puts the soonest event in &e and removes it from the queue
// returns 0 if no more events
// returns 1 if an event was successfully removed
int InputEventQueue::extract_soonest (input_event_t *e) {
  if (q.size() < 1) {
    // signal that there are no more events
    return 0; }

  if (!sorted) {
    // sort the bugger
    sort (q.begin (), q.end (), &InputEventQueue::compare_time);

    sorted = true;
  }

  // stick the info in the pointer
  *e = q[q.size() - 1];

  // remove the last element in the list
  q.pop_back ();

  // success
  return 1;
}


// returns the time of the next event
// returns -1.0 if no next event
double InputEventQueue::time_next (void) {
  if (q.size() < 1) {
    // signal that there are no more events
    return -1.0; }

  if (!sorted) {
    // sort the bugger
    sort (q.begin (), q.end (), &InputEventQueue::compare_time);

    sorted = true;
  }

  // return the time
  return q[q.size() - 1].time;
}


#endif // _InputEventQueue_h
