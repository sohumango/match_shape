#ifndef _W1_H
#define _W1_H

void rgb2bgr(unsigned char src[], int w, int h, int channel);
int imshow( unsigned char * buff, int w, int h, int channel, bool bbgr2rgb);

#endif
