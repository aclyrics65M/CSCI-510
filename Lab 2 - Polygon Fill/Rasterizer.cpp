//
//  Rasterizer.cpp
//
//  Created by Joe Geigel on 11/30/11.
//  Copyright 2011 Rochester Institute of Technology. All rights reserved.
//
//  Contributor:  Guillaume Roy
//
//  To do this exercise, I helped myself with the following website:
//  http://www.cs.rit.edu/~icss571/filling/index.html and adapted the
//  algorithm explained into it to C++ source code.
//

#include "Rasterizer.h"
#include "simpleCanvas.h"

using namespace std;

///
//
// Simple class that performs rasterization algorithms
//
///

///
// Constructor
//
// @param n - number of scanlines
//
///

Rasterizer::Rasterizer (int n) : n_scanlines (n)
{
}

///
// Draw a filled polygon in the simpleCanvas C.
//
// The polygon has n distinct vertices.  The coordinates of the vertices
// making up the polygon are stored in the x and y arrays.  The ith
// vertex will have coordinate (x[i],y[i]).
//
// You are to add the implementation here using only calls
// to C.setPixel()
///

void Rasterizer::drawPolygon(int n, int x[], int y[], simpleCanvas &C)
{
    // YOUR IMPLEMENTATION GOES HERE


	/*
		Initializing All of the Edges
	*/
	
	// we define a Bucket structure to store, for each edge, the following values
	typedef struct {
		int yMin, // The minimum y value of the two vertices
			yMax; // The maximum y value of the two vertices
		float xVal, // The x value associated with the minimum y value
			  inv_m; // 1/m, the inverse of the slope
	} Bucket ;
	
	// we allocate an Array that will contain all the Buckets of the polygon
	vector<Bucket> *globalEdgeTable = new vector<Bucket>();

	for (int i = 0; i < n - 1; i++) {
		Bucket b;
		// let's initialize the current bucket
		b.yMin = min(y[i], y[i + 1]);
		b.yMax = max(y[i], y[i + 1]);
		b.xVal = b.yMin == y[i] ? x[i] : x[i + 1];
		b.inv_m = b.yMax - b.yMin != 0 ?
			(x[i] - x[i + 1]) / (y[i] - y[i + 1]) : FLT_MAX;
		// we don't need to store horizontal lines. As we have the 
		if (b.inv_m != FLT_MAX)
			globalEdgeTable->push_back(b);
	}

	// here is the number of buckets we truly keep after having removed the horizontal edges
	n = globalEdgeTable->size();
	

	/*
		Initializing the Global Edge Table
	*/

	Bucket b;

	// we need to sort the buckets...
	for (int i = 0; i < n - 1; i++) {
		for (int j = i; j < n - 1; j++) {
			// sort by yMin...
			if (globalEdgeTable->at(j).yMin <= globalEdgeTable->at(i).yMin) {
				// or by xVal if yMin is the same
				if (globalEdgeTable->at(j).yMin < globalEdgeTable->at(i).yMin || globalEdgeTable->at(j).xVal < globalEdgeTable->at(i).xVal) {
					b = globalEdgeTable->at(j);
					globalEdgeTable->at(j) = globalEdgeTable->at(i);
					globalEdgeTable->at(i) = b;
				}
			}
		}
	}


	/*
		Initializing parity
	*/

	int parity = 0;


	/*
		Initializing the Scan-Line 
	*/

	int scanLine = globalEdgeTable->at(0).yMin;


	/*
		Initializing the Active Edge Table
	*/

	vector<Bucket> *activeEdgeTable = new vector<Bucket>();

	/*
	Since the global edge table is ordered on minimum y and x values,
	search, in order, through the global edge table and, for each edge 
	found having a minimum y value equal to the current scan-line, append 
	the edge information for the maximum y value, x value, and 1/m to the 
	active edge table. Do this until an edge is found with a minimum y 
	value greater than the scan line value. The active edge table will 
	now contain ordered edges of those edges that are being filled as such: 
	*/

	int maxLine = globalEdgeTable->at(n-1).yMax;

	for (int line = scanLine; line < maxLine; line++) {
		for (int i = 0; i < n - 1; i++) {
			b = globalEdgeTable->at(i);
			if (b.yMin == scanLine) {
				activeEdgeTable->push_back(b);
				globalEdgeTable->erase(globalEdgeTable->begin() + i);
			}
		}
		free(activeEdgeTable);
		activeEdgeTable = new vector<Bucket>();
	}


	free(globalEdgeTable);
	free(activeEdgeTable);
}