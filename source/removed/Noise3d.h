// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Noise3d
// *
// * deprecated - please remove
// *
// * not sure that this actually does anything
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Noise3d_h_
#define Noise3d_h_

#include <malloc.h>

#include "v2d.h"

#include "terrain.h"

#include "simplex.c"


// defines * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
#define DEFAULT_FIELD_SIDE			(32)

#define DEFAULT_TERRAIN_OFFSET		(16.0)


//  class definition * * * * * * * * * * * * * * * * * * * * * * * 
class Noise3d {
public:
	Noise3d (void);
	Noise3d (int side);
	~Noise3d (void);

	int resize (int side);

	int get_side_length (void);
	
	void generateTilable (double delta);

	void normalize (double low, double high);

	double get_value (int i, int j, int k);
	double getValueTrilerp (double i, double j, double k);

	int set_value (int i, int j, int k, double value);

	int set_all (double value);
	int set_min (double min);

	int smooth (void);
	void noise (int octaves);
	void noiseFrom2d (void);
	double blurCube (int i, int j, int k, int side);

	void alt_smooth (void);

	double getMax (void);
	double getMin (void);

	// gives the value of the lowest neighbor of the eight surrounding points
	double lowest_neighbor (int i, int j, int k);


private:
	int mSideLength;
	int mSideLengthSquared;

	double *field;
};


// **************************************************
//	default constructor
// **************************************************
Noise3d::Noise3d (void) {
	field = NULL;
	mSideLength = 0;
	mSideLengthSquared = 0;

	// allocate the field and set to zero
	if (resize (DEFAULT_FIELD_SIDE) == 0) {
		// clear the field to zeros
		set_all (0.0);
	}
}


// **************************************************
//	constructor that takes a mSideLength value as an argument
// **************************************************
Noise3d::Noise3d (int side) {
	field = NULL;
	mSideLength = 0;
	mSideLengthSquared = 0;

	// allocate the field and set to zero
	if (resize (side) == 0) {
		// clear the field to zeros
		set_all (0.0);
	}
}


// **************************************************
//	destructor
// **************************************************
Noise3d::~Noise3d (void) {
	if (field != NULL) free (field);
}


// **************************************************
//	resize the terrain
// **************************************************
int Noise3d::resize (int side) {
	// if the field is already allocated and of the same size, nothing to do
	if (mSideLength == side && field != NULL) {
		return 0;
	}

	// free the old field
	if (field != NULL) free (field);
	
	// allocate the field
	field = (double *)malloc (sizeof (double) * side * side * side);

	// if the malloc failed, set field side to zero and bail
	if (field == NULL) {
		mSideLength = 0;
		return -1;
	}
	// malloc succeeded
	else {
		mSideLength = side;
		mSideLengthSquared = side * side;
	}

	// success
	return 0;
}


// **************************************************
//	returns the side length of the volume
// **************************************************
int Noise3d::get_side_length (void) {
	return mSideLength;
}


// **************************************************
//	start the subdivision process
//		set one corner as seed
// **************************************************
void Noise3d::generateTilable (double delta) {
	// bail if we've got nothing to work with
/*	if (field == NULL || mSideLength < 2) return;

	// seed bl corner
	set_value (0, 0, 0, 0.0);
//	set_all (0.0);

	double avg;
	v3di_t bl;
	int add = mSideLength >> 1;
	int addTimesTwo;

	// get'r done
	while (add > 0) {
		addTimesTwo = add << 1;

		// squares
		for (bl.z = 0; bl.z < mSideLength; bl.z += addTimesTwo) {
			for (bl.y = 0; bl.y < mSideLength; bl.y += addTimesTwo) {
				for (bl.x = 0; bl.x < mSideLength; bl.x += addTimesTwo) {
					avg = (get_value (bl.x, bl.y, bl.z) +
							get_value (bl.x, bl.y, bl.z + addTimesTwo) +
							get_value (bl.x, bl.y + addTimesTwo, bl.z) +
							get_value (bl.x, bl.y + addTimesTwo, bl.z + addTimesTwo) +
							get_value (bl.x + addTimesTwo, bl.y, bl.z) +
							get_value (bl.x + addTimesTwo, bl.y, bl.z + addTimesTwo) +
							get_value (bl.x + addTimesTwo, bl.y + addTimesTwo, bl.z) +
							get_value (bl.x + addTimesTwo, bl.y + addTimesTwo, bl.z + addTimesTwo)) * 0.125;

					set_value (bl.x + add, bl.y + add, bl.z + add, get_close (avg, delta));
				}
			}
		}

		// diamonds
		for (bl.z = 0; bl.z < mSideLength; bl.z += addTimesTwo) {
			for (bl.y = 0; bl.y < mSideLength; bl.y += addTimesTwo) {
				for (bl.x = 0; bl.x < mSideLength; bl.x += addTimesTwo) {
					// diamond: left
					avg = (get_value (bl.x, bl.y + addTimesTwo) +
							get_value (bl.x + add, bl.y + add) +
							get_value (bl.x, bl.y) +
							get_value (bl.x - add, bl.y + add)) * 0.25;

					set_value (bl.x, bl.y + add, get_close (avg, delta));

					// diamond: bottom
					avg = (get_value (bl.x + add, bl.y + add) +
							get_value (bl.x + addTimesTwo, bl.y) +
							get_value (bl.x + add, bl.y - add) +
							get_value (bl.x, bl.y)) * 0.25;

					set_value (bl.x + add, bl.y, get_close (avg, delta));
				}
			}
		}

		add >>= 1;
		delta *= 0.6;
	}
	*/

}





void Noise3d::normalize (double low, double high) {
	double newRange = high - low;
	double oldMin = getMin ();
	double oldMax = getMax ();

	for (int k = 0; k < mSideLength; k++) {
		for (int j = 0; j < mSideLength; j++) {
			for (int i = 0; i < mSideLength; i++) {
				double oldValue = get_value (i, j, k);

				set_value (i, j, k, (newRange * (oldValue - oldMin) / (oldMax - oldMin)) + low);
			}
		}
	}

}




// **************************************************
//	return the value of field at (i, j)
//	return 0.0 on error
// **************************************************
double Noise3d::get_value (int i, int j, int k) {
/*	// off the map, or mSideLength == 0
	if (x < 0 || x >= mSideLength ||
		z < 0 || z >= mSideLength) return 0.0;*/

	if (field == NULL || mSideLength == 0) return 0.0;

	// ensure the i coord is on the map
	while (i < 0) i += mSideLength;
	if (i >= mSideLength) {
		i = i % mSideLength;
	}

	// ensure the j coord is on the map
	while (j < 0) j += mSideLength;
	if (j >= mSideLength) {
		j = j % mSideLength;
	}

	// ensure the k coord is on the map
	while (k < 0) k += mSideLength;
	if (k >= mSideLength) {
		k = k % mSideLength;
	}

	return field[i + (j * mSideLength) + (k * mSideLengthSquared)];
}



double Noise3d::getValueTrilerp (double i, double j, double k) {
	if (field == NULL || mSideLength == 0) return 0.0;

	double side = static_cast<double>(mSideLength);

	// ensure the the coords are on the map
	while (i < 0.0) i += side;
	if (i >= (side)) {
		i = fmod (i, side);
	}

	while (j < 0.0) j += side;
	if (j >= (side)) {
		j = fmod (j, side);
	}

	while (k < 0.0) k += side;
	if (k >= (side)) {
		k = fmod (k, side);
	}

	v3di_t nearCorner;
	v3di_t farCorner;
	v3d_t unitPosition;

	nearCorner.x = static_cast<int>(floor (i));
	nearCorner.y = static_cast<int>(floor (j));
	nearCorner.z = static_cast<int>(floor (k));

	unitPosition.x = i - static_cast<double>(nearCorner.x);
	unitPosition.y = j - static_cast<double>(nearCorner.y);
	unitPosition.z = k - static_cast<double>(nearCorner.z);

	farCorner.x = (nearCorner.x + 1) % mSideLength;
	farCorner.y = (nearCorner.y + 1) % mSideLength;
	farCorner.z = (nearCorner.z + 1) % mSideLength;

//	v3di_print ("near", nearCorner);
//	v3di_print ("far", farCorner);
//	v3d_print ("unit", unitPosition);

	double vert000 = field[nearCorner.x + (nearCorner.y * mSideLength) + (nearCorner.z * mSideLengthSquared)];
	double vert001 = field[nearCorner.x + (nearCorner.y * mSideLength) + (farCorner.z * mSideLengthSquared)];
	double vert010 = field[nearCorner.x + (farCorner.y * mSideLength) + (nearCorner.z * mSideLengthSquared)];
	double vert011 = field[nearCorner.x + (farCorner.y * mSideLength) + (farCorner.z * mSideLengthSquared)];
	double vert100 = field[farCorner.x + (nearCorner.y * mSideLength) + (nearCorner.z * mSideLengthSquared)];
	double vert101 = field[farCorner.x + (nearCorner.y * mSideLength) + (farCorner.z * mSideLengthSquared)];
	double vert110 = field[farCorner.x + (farCorner.y * mSideLength) + (nearCorner.z * mSideLengthSquared)];
	double vert111 = field[farCorner.x + (farCorner.y * mSideLength) + (farCorner.z * mSideLengthSquared)];

	// this was taken from: http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/index.html
	// section on trilinear interpolation
	return (vert000 * (1.0 - unitPosition.x) * (1.0 - unitPosition.y) * (1.0 - unitPosition.z)) +
		(vert100 * (unitPosition.x) * (1.0 - unitPosition.y) * (1.0 - unitPosition.z)) +
		(vert010 * (1.0 - unitPosition.x) * (unitPosition.y) * (1.0 - unitPosition.z)) +
		(vert001 * (1.0 - unitPosition.x) * (1.0 - unitPosition.y) * (unitPosition.z)) +
		(vert101 * (unitPosition.x) * (1.0 - unitPosition.y) * (unitPosition.z)) +
		(vert011 * (1.0 - unitPosition.x) * (unitPosition.y) * (unitPosition.z)) +
		(vert110 * (unitPosition.x) * (unitPosition.y) * (1.0 - unitPosition.z)) +
		(vert111 * (unitPosition.x) * (unitPosition.y) * (unitPosition.z));
}



// **************************************************
//	set the value at (i, j)
//	return 0 on success, -1 on error
// **************************************************
int Noise3d::set_value (int i, int j, int k, double value) {
	if (i < 0 || i >= mSideLength ||
		j < 0 || j >= mSideLength ||
		k < 0 || k >= mSideLength) return -1;

	field[i + (j * mSideLength) + (k * mSideLengthSquared)] = value;

	return 0;
}


// **************************************************
//	set the entire field to some value
// **************************************************
int Noise3d::set_all (double value) {

	for (int i = 0; i < mSideLength * mSideLengthSquared; i++) {
		field[i] = value;
	}

	return 0;
}


// **************************************************
//	if any elements in the field are below a certain number, set them to that number
// **************************************************
int Noise3d::set_min (double min) {
/*	for (int j = 0; j < mSideLength; j++) {
		for (int i = 0; i < mSideLength; i++) {
			// get the value at i, j
			double val = get_value (i, j);

			// if it's lower than our min, set it's new value
			if (val < min) {
				set_value (i, j, min);
			}
		}
	}*/

	return 0;
}


// **************************************************
//	use a simple blur filter to smooth the field
// **************************************************
int Noise3d::smooth (void) {
	// initialize a scratch buffer
	double *buf = (double *)malloc (sizeof (double) * mSideLength * mSideLengthSquared);

	// bail if that didn't work
	if (buf == NULL) return -1;

	// now fill the scratch buffer up with smoothed values
	for (int k = 0; k < mSideLength; k++) {
		for (int j = 0; j < mSideLength; j++) {
			for (int i = 0; i < mSideLength; i++) {
				buf[i + (j * mSideLength) + (k * mSideLengthSquared)] = blurCube (i, j, k, 1);
			}
		}
	}

	// set the real field values
	for (int k = 0; k < mSideLength; k++) {
		for (int j = 0; j < mSideLength; j++) {
			for (int i = 0; i < mSideLength; i++) {
				set_value (i, j, k, buf[i + (j * mSideLength) + (k * mSideLengthSquared)]);
			}
		}
	}

	// free up the scratch buffer
	free (buf);

	// success
	return 0;
}


void Noise3d::noise (int octaves) {
	double low = 1000.00;
	double high = -1000.00;
	for (int k = 0; k < mSideLength; k++) {
		for (int j = 0; j < mSideLength; j++) {
			for (int i = 0; i < mSideLength; i++) {
				double iF = 5.0 * static_cast<double>(i) / static_cast<double>(mSideLength);
				double jF = 5.0 * static_cast<double>(j) / static_cast<double>(mSideLength);
				double kF = 5.0 * static_cast<double>(k) / static_cast<double>(mSideLength);

//				iF = 1.0f - (2.0f * iF);
//				jF = 1.0f - (2.0f * jF);
//				kF = 1.0f - (2.0f * kF);

				double v = simplex_noise (octaves, iF, jF, kF);

				if (v > high) high = v;
				if (v < low) low = v;

				set_value (i, j, k, v);
			}
		}
	}
//	printf ("simplex: low: (%2.3f), high: (%2.3f)\n", low, high);
}



void Noise3d::noiseFrom2d (void) {
	terrain_c x, y, z;


	x.resize (mSideLength);
	y.resize (mSideLength);
	z.resize (mSideLength);

	x.generateTilable (32.0);
	y.generateTilable (32.0);
	z.generateTilable (32.0);

	x.normalize (0.0, 1.0);
	y.normalize (0.0, 1.0);
	z.normalize (0.0, 1.0);

	for (int k = 0; k < mSideLength; k++) {
		for (int j = 0; j < mSideLength; j++) {
			for (int i = 0; i < mSideLength; i++) {
				double xVal = x.get_value (j, k);
				double yVal = y.get_value (i, k);
				double zVal = z.get_value (i, j);

				set_value (i, j, k, xVal + yVal + zVal);
			}
		}
	}
}



void Noise3d::alt_smooth (void) {
	// initialize a scratch buffer
/*	double *buf = (double *)malloc (sizeof (double) * mSideLength * mSideLength);

	// bail if that didn't work
	if (buf == NULL) return;

	double minValue = getMin ();
	double maxValue = getMax ();
	double range = maxValue - minValue;

	if (range == 0.0) range = 1.0;

	// now fill the scratch buffer up with smoothed values
	for (int j = 0; j < mSideLength; j++) {
		for (int i = 0; i < mSideLength; i++) {
			double orig = get_value (i, j);
			double blur = blur_square (i, j, 4);

			double percent = (0.6 * (maxValue - orig) / range) + 0.4;
			percent = percent * percent * percent;

			buf[i + (j * mSideLength)] = lerp (orig, blur, percent);
		}
	}

	// set the real field values
	for (int j = 0; j < mSideLength; j++) {
		for (int i = 0; i < mSideLength; i++) {
			set_value (i, j, buf[i + (j * mSideLength)]); } }

	// free up the scratch buffer
	free (buf);

*/
}



double Noise3d::blurCube (int i, int j, int k, int side) {
	double total = 0.0;
	int count = 0;

	for (int c = k - side; c <= k + side; c++) {
		for (int b = j - side; b <= j + side; b++) {
			for (int a = i - side; a <= i + side; a++) {
				total += get_value (a, b, c);
				count++;
			}
		}
	}

	double sideDouble = static_cast<double>(side);

	total += (sideDouble * sideDouble * get_value (i, j, k));
	count += (side * side);

	return total / static_cast<double>(count);
}



double Noise3d::getMax (void) {
/*	double value;
	double maxValue = field[0];

	for (int j = 0; j < mSideLength; j++) {
		for (int i = 0; i < mSideLength; i++) {
			if ((value = get_value (i, j)) > maxValue) {
				maxValue = value;
			}
		}
	}

	return maxValue;*/

	return 0.0;
}



double Noise3d::getMin (void) {
/*	double value;
	double minValue = field[0];

	for (int j = 0; j < mSideLength; j++) {
		for (int i = 0; i < mSideLength; i++) {
			if ((value = get_value (i, j)) < minValue) {
				minValue = value;
			}
		}
	}

	return minValue;*/

	return 0.0;
}


// gives the value of the lowest neighbor of the 4 surrounding points
double Noise3d::lowest_neighbor (int i, int j, int k) {
/*	double lowest = 100000000.0;

//	if (get_value (i - 1, j - 1) < lowest) lowest = get_value (i - 1, j - 1);
	if (get_value (i,     j - 1) < lowest) lowest = get_value (i,     j - 1);
//	if (get_value (i + 1, j - 1) < lowest) lowest = get_value (i + 1, j - 1);
	if (get_value (i - 1, j    ) < lowest) lowest = get_value (i - 1, j    );
	if (get_value (i + 1, j    ) < lowest) lowest = get_value (i + 1, j    );
//	if (get_value (i - 1, j + 1) < lowest) lowest = get_value (i - 1, j + 1);
	if (get_value (i,     j + 1) < lowest) lowest = get_value (i,     j + 1);
//	if (get_value (i + 1, j + 1) < lowest) lowest = get_value (i + 1, j + 1);

	return lowest;*/

	return 0.0;
}



#endif // Noise3d_h_
