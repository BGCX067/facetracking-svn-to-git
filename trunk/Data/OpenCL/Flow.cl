void conv(float *, float *, float *, int2, int2);

/******************************************************************************/

float kernelA[] = {1,0,0,0};

/******************************************************************************/

__kernel void  flow(__global float *img1, 
					__global float *img2,
					__global float *outX,
					__global float *outY,
					 int imgH,
					 int imgW) {
					 
	float aCopy[imgH*imgW], bCopy[imgH*imgW];
	float xCopy[imgH*imgW], yCopy[imgH*imgW];
	for(int i=0; i<imgH*imgW; i++) {
		aCopy[i] = img1[i];
		bCopy[i] = img2[i];

	}


	for(int i=0; i<imgH*imgW; i++) {
		//xCopy[1]=0;
		//outY[i] = yCopy[i];
	}
}

/******************************************************************************/

void conv(float *A, float *B, float *C, int2 dataDim, int2 kernelDim) {

}
