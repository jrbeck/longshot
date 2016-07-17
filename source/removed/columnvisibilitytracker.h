#ifndef columnvisibilitytracker_h_
#define columnvisibilitytracker_h_


//    this thing keeps a list of whether columns in a WorldMap
// are currently being displayed.
//    it is implemented as a vector containing a column index (v2di_t)
// and a type, which is whether the particular column is being displayed or hidden
// in the bloctree program.
//    specifically in bloctree, this allows the worldmap to communicate this info to
// the physics/ai/item/... engines to let them know that they should take appropriate
// action for any of these possible events


enum {
  COLUMN_VIZ_NULL = -1,
  COLUMN_VIZ_SHOW,
  COLUMN_VIZ_HIDE
};


typedef struct {
  int type;
  v2di_t index;
} column_viz_t;


class ColumnVisibilityTracker {
private:
  vector <column_viz_t> mColumns;

public:
  ColumnVisibilityTracker ();
  ~ColumnVisibilityTracker ();

  void clear (void);

  void showColumn (v2di_t index);
  void hideColumn (v2di_t index);

  void addColumn (column_viz_t column);

  column_viz_t popColumn (void);
};



ColumnVisibilityTracker::ColumnVisibilityTracker () {
}



ColumnVisibilityTracker::~ColumnVisibilityTracker () {
  clear ();
}



void ColumnVisibilityTracker::clear (void) {
  mColumns.clear ();
}



void ColumnVisibilityTracker::showColumn (v2di_t index) {
  column_viz_t column;

  column.type = COLUMN_VIZ_SHOW;
  column.index = index;

  v2di_print ("---- show column ", column.index);
  addColumn (column);
}



void ColumnVisibilityTracker::hideColumn (v2di_t index) {
  column_viz_t column;

  column.type = COLUMN_VIZ_HIDE;
  column.index = index;

  v2di_print ("---- hide column ", column.index);
  addColumn (column);
}



void ColumnVisibilityTracker::addColumn (column_viz_t column) {
  for (size_t i = 0; i < mColumns.size(); ++i) {
    if (column.index.x == mColumns[i].index.x &&
      column.index.y == mColumns[i].index.y) {

      if (mColumns[i].type == column.type) {
        printf ("error: ColumnVisibilityTracker::addColumn() wants to double add (%d)...ignoring\n\n", column.type);
        return;
      }
      else {
        // two oppposite calls cancel out
        if (i != mColumns.size() - 1) {
          swap (mColumns[i], mColumns[mColumns.size() - 1]);
        }

        printf ("i never had a chance!\n");

        mColumns.pop_back ();
        return;
      }
    }
  }

  mColumns.push_back (column);
}



column_viz_t ColumnVisibilityTracker::popColumn (void) {
  column_viz_t column;

  if (mColumns.size() == 0) {
    column.type = COLUMN_VIZ_NULL;
    return column;
  }

  column = mColumns[mColumns.size() - 1];
  mColumns.pop_back ();

//  printf ("* * * * column popped!\n");

  return column;
}



#endif // columnvisibilitytracker_h_
