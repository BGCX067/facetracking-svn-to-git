// Gets pixel value if possible
int arr(__global int *image,
	const int y,
	const int x,
	const int height,
	const int width){
	if(y<0 || y>=height || x<0 || x>=width)
		return 0xff;
	return image[y*width + x];
}

__kernel void  warp(	
	__global float *coords,	// Matrix Nx6 of N affine coordinates
	const int bW,			// Width of warped samples
	const int bH,			// Height of warped samples
	__global int *image,	// MxN image (BW range 0-255)
	const int iW,			// Width of image
	const int iH,			// Height of image
	__global int *output,	// 48*Nx48 warped images
	const int coordCount,	// Number of coordinates to warp
	const int workUnits)	// Number of coordinates each thread must compute
	{

	int id = get_global_id(0);
	
	// Check thread has any samples to compute 
	if(id>=coordCount)	
		return;
	
	for(int taskID=id*workUnits; taskID<coordCount && taskID < (id + 1)*workUnits; taskID++) {
		for(int x=0; x<bW; x++) {
			for(int y=0; y<bH; y++) {
				int cx = x - bW/2;
				int cy = y - bH/2;
				float tx = coords[taskID*6+0] + cx * coords[taskID*6+2] + cy * coords[taskID*6+3];
				float ty = coords[taskID*6+1] + cx * coords[taskID*6+4] + cy * coords[taskID*6+5];
				
				float x1 = tx - 1;
				float x2 = tx + 1;
				float y1 = ty - 1; 
				float y2 = ty + 1;
				float r1 = (x2 - tx)/(x2 - x1)*arr(image,y1,x1,iH,iW)+(tx - x1)/(x2 - x1)*arr(image,y1,x2,iH,iW);
				float r2 = (x2 - tx)/(x2 - x1)*arr(image,y2,x1,iH,iW)+(tx - x1)/(x2 - x1)*arr(image,y2,x2,iH,iW);

				output[taskID*48*48 + y*48 + x] = (y2 - ty)/(y2 - y1)*r1 + (ty - y1)/(y2 - y1)*r2;	
			}
		}
	}
}