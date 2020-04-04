#ifndef UTILITIES_H
#define UTILITIES_H


// Indices (location) of queue families (if they exist at all)
struct QueueFamiliesIndices {
	int graphicsFamily = -1;
	int presentationFamily = -1;

	// Check if Queue families are valid
	bool isValid()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

#endif // UTILITIES_H
