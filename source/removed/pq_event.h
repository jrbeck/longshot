/*

void process_events (double process_until) {
  event_t temp;

  // send input signals if it is time
  while (process_until > next_input_time) {
    draw_input_flag = 3;
    temp.time = next_input_time;
    next_input_time += INPUT_DELAY;
    for (int i = 0; i < (int)input_neuron.size (); i++) {
      temp.target = input_neuron[i];
      temp.charge = input_level;

      event_insert (event_list, temp);

//      printf ("%d: %f @ %f\n", e[i].target, e[i].charge, e[i].time);
    }
  }

  // process all the events which occur in this time frame
  while (event_list.size () > 0 && event_list[0].time <= process_until) {
    event_t cur = event_extract_soonest (event_list);

    // just to make things a touch easier to write
    int t = cur.target;

//    printf ("%5.3f: (q_size: %d): target:%d, targ charge:%.2f, syn charge:%.2f, targ thresh:%.2f\n", cur.time, e.size (), t, neuron[t].charge, cur.charge,
//      neuron[t].threshold);

    // handle the decay of the target neuron's charge
    neuron[t].charge -= ((cur.time - neuron[t].last_decay) * DECAY_RATE);
    neuron[t].last_decay = cur.time;

    if (neuron[t].charge < 0.0f) neuron[t].charge = 0.0f;

    // add the charge from the synapse
    neuron[t].charge += cur.charge;

//    printf ("post decay targ chg: %.2f\n", neuron[t].charge);

    // does the neuron fire?
    if (neuron[t].charge >= neuron[t].threshold) {
      // update the stats
      if (neuron[t].soma.x > furthest_right_fire) furthest_right_fire = neuron[t].soma.x;
      num_fires++;

      // send a signal through each synapse
      for (int j = 0; j < (int)neuron[t].synapse.size (); j++) {
        temp.target = neuron[t].synapse[j].target;
        temp.charge = neuron[t].threshold * neuron[t].synapse[j].percent;
        temp.time = cur.time + (neuron[t].synapse[j].length * (1.0f / SIGNAL_PROPOGATION_RATE));

        event_insert (event_list, temp);

//        printf ("fire: %d, chg: %.2f, arrives at: %.3f\n", temp.target, temp.charge, temp.time);

        neuron[t].flash_time = cur.time + FLASH_TIME;
      }

      // reset the charge
      neuron[t].charge = 0.0f;
    }
  }

//  printf ("%d, ", e.size ());
}


*/



typedef struct {
  double time;

  void *data;
} pq_event_t;


class pq_event_c {
  vector<pq_event_t> heap;

  int num_events;

public:
  pq_event_c ();
  ~pq_event_c ();

  void insert (pq_event_t e);
  pq_event_t extract_soonest (void);

  void decrease_key (int i);
  void min_heapify (int i);

  inline int heap_parent (int i);
  inline int heap_left (int i);
  inline int heap_right (int i);
};



pq_event_c::pq_event_c () {
}


pq_event_c::~pq_event_c () {
}




// insert an event on the queue
void pq_event_c::insert (pq_event_t e) {
  heap.push_back (e);

  decrease_key ((int)heap.size () - 1);

  // increment the global counter
  num_events++;
}


pq_event_t pq_event_c::extract_soonest (void) {
  // this better not happen!
  assert (heap.size () > 0);

  pq_event_t  ret = heap[0];

  heap[0] = heap[heap.size () - 1];
  heap.pop_back ();

  min_heapify (0);

  return ret;
}

void pq_event_c::decrease_key (int i) {
  int parent;

  while (i > 0 && heap[parent = heap_parent (i)].time > heap[i].time) {
    // swap!
    pq_event_t temp = heap[i];
    heap[i] = heap[parent];
    heap[parent] = temp;

    i = parent;
  }
}

void pq_event_c::min_heapify (int i) {
  int l = heap_left (i);
  int r = heap_right (i);

  int smallest;

  if (l < (int)heap.size () && heap[l].time < heap[i].time) smallest = l;
  else smallest = i;

  if (r < (int)heap.size () && heap[r].time < heap[smallest].time) smallest = r;

  if (smallest != i) {
    // swap!
    pq_event_t temp = heap[i];
    heap[i] = heap[smallest];
    heap[smallest] = temp;

    min_heapify (smallest);
  }
}



inline int pq_event_c::heap_parent (int i) {
  return ((i + 1) >> 1) - 1;
}

inline int pq_event_c::heap_left (int i) {
  return ((i + 1) << 1) - 1;
}

inline int pq_event_c::heap_right (int i) {
  return (i + 1) << 1;
}
