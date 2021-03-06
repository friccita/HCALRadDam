#ifndef SimG4CMS_HEDarkening_h
#define SimG4CMS_HEDarkening_h

#include <cmath>
#include <iostream>

#define maxEta 14
#define maxLay 19

//root version of HEDarkening
class HEDarkening {

public:
  HEDarkening(int mode_=0) {
    //mode: 0 = default, 1 = fingers, 2 = power law
	mode = mode_;
    
    //HE starts at tower 16
    ieta_shift = 16;
    
    //finger tiles start at tower 25 (arbitrarily decided)
    //finger_ieta = 25 - ieta_shift;
    finger_ieta = 16 - ieta_shift;
    //finger tile scale parameter (attenuation) in MRad from NIM A717 (2013) 11-13
    //finger_scale = 7.27;
    finger_scale = 6.4;
    //dose rate:
	//1E33 cm^-2 s^-1 for Phase 0 (avg, 7.7E33 peak)
    //1E34 cm^-2 s^-1 for Phase 1
    //5E34 cm^-2 s^-1 for Phase 2 <-- use this one
    //convert to fb-1/hr
    //inst_lumi = 0.18;
    inst_lumi = 0.004;
    
    //scale parameter in fb-1 for exponential darkening from radiation damage for each tile
    //fits from laser data for L1 and L7
    //L0,Lm1 = L1, L2-L6 interpolated, L8-L17 extrapolated
    //from Vladimir Epshteyn
    float _lumiscale[maxEta][maxLay] = {
        {1194.9, 1194.9, 1194.9, 1651.5, 2282.7, 3155.2, 4361.0, 6027.8, 8331.5, 11515.7, 15916.8, 22000.0, 30408.2, 42029.8, 58093.1, 80295.6, 110983.5, 153400.1, 212027.7},
        {952.8, 952.8, 952.8, 1293.9, 1757.1, 2386.1, 3240.3, 4400.3, 5975.4, 8114.5, 11019.3, 14963.9, 20320.6, 27594.9, 37473.2, 50887.7, 69104.3, 93841.9, 127435.0},
        {759.8, 759.8, 759.8, 1013.8, 1352.5, 1804.5, 2407.6, 3212.2, 4285.7, 5717.9, 7628.7, 10178.1, 13579.5, 18117.6, 24172.3, 32250.4, 43028.0, 57407.4, 76592.2},
        {605.9, 605.9, 605.9, 794.2, 1041.1, 1364.7, 1788.9, 2344.9, 3073.7, 4029.1, 5281.4, 6922.9, 9074.7, 11895.2, 15592.4, 20438.8, 26791.5, 35118.8, 46034.2},
        {483.2, 483.2, 483.2, 622.3, 801.4, 1032.1, 1329.2, 1711.8, 2204.5, 2839.1, 3656.3, 4708.8, 6064.3, 7809.9, 10058.0, 12953.2, 16681.8, 21483.8, 27667.9},
        {385.3, 385.3, 385.3, 487.5, 616.9, 780.5, 987.6, 1249.6, 1581.1, 2000.6, 2531.3, 3202.8, 4052.5, 5127.6, 6487.9, 8209.2, 10387.0, 13142.6, 16629.3},
        {307.3, 307.3, 307.3, 382.0, 474.8, 590.3, 733.8, 912.2, 1134.0, 1409.7, 1752.4, 2178.5, 2708.1, 3366.6, 4185.1, 5202.6, 6467.5, 8039.9, 9994.7},
        {245.0, 245.0, 245.0, 299.3, 365.5, 446.4, 545.2, 665.9, 813.3, 993.3, 1213.2, 1481.8, 1809.7, 2210.3, 2699.6, 3297.2, 4027.0, 4918.4, 6007.1},
        {195.4, 195.4, 195.4, 234.5, 281.3, 337.6, 405.1, 486.1, 583.3, 700.0, 839.9, 1007.9, 1209.4, 1451.2, 1741.4, 2089.6, 2507.4, 3008.8, 3610.5},
        {155.8, 155.8, 155.8, 183.7, 216.6, 255.3, 301.0, 354.9, 418.4, 493.2, 581.5, 685.5, 808.2, 952.8, 1123.3, 1324.3, 1561.3, 1840.6, 2170.0},
        {124.3, 124.3, 124.3, 143.9, 166.7, 193.1, 223.6, 259.0, 300.1, 347.5, 402.6, 466.3, 540.1, 625.6, 724.6, 839.3, 972.1, 1126.0, 1304.2},
        {99.1, 99.1, 99.1, 112.8, 128.3, 146.0, 166.2, 189.1, 215.2, 244.9, 278.7, 317.2, 360.9, 410.7, 467.4, 531.9, 605.3, 688.8, 783.9},
        {79.0, 79.0, 79.0, 88.3, 98.8, 110.4, 123.5, 138.0, 154.3, 172.6, 192.9, 215.7, 241.2, 269.7, 301.5, 337.1, 376.9, 421.4, 471.1},
        {63.0, 63.0, 63.0, 69.2, 76.0, 83.5, 91.7, 100.8, 110.7, 121.6, 133.6, 146.7, 161.2, 177.0, 194.5, 213.6, 234.7, 257.8, 283.2}
    };
    
    //dose map in MRad/fb-1 from MARS, sqrt(s) = 14 TeV 
    float _towerDose[maxEta][maxLay] = {
        {0,0,0,0,1.0979e-05,6.0286e-06,4.128e-06,2.3513e-06,1.2164e-06,7.7354e-07,0,0,0,0,0,0,0,0,0},
        {4.6451e-05,4.6451e-05,4.6451e-05,2.7681e-05,1.796e-05,1.2062e-05,7.8485e-06,4.7856e-06,3.2006e-06,1.7051e-06,1.1322e-06,8.413e-07,4.0945e-07,4.0107e-07,2.5711e-07,0,0,0,0},
        {8.449e-05,8.449e-05,8.449e-05,5.6394e-05,3.6206e-05,2.3624e-05,1.5589e-05,8.3604e-06,5.3963e-06,3.3172e-06,2.2294e-06,1.3811e-06,7.4775e-07,4.6541e-07,3.9259e-07,3.3171e-07,1.21e-07,0,0},
        {0.0001298,0.0001298,0.0001298,8.8925e-05,6.179e-05,3.9852e-05,2.8805e-05,1.567e-05,1.0071e-05,8.1823e-06,4.2244e-06,2.4281e-06,1.5235e-06,9.8017e-07,6.2373e-07,4.6479e-07,2.0052e-07,2.3193e-07,3.4805e-07},
        {0.00018969,0.00018969,0.00018969,0.00013045,9.5167e-05,6.3313e-05,4.1003e-05,2.5423e-05,1.5999e-05,1.0608e-05,6.6461e-06,4.6462e-06,3.7258e-06,1.87e-06,1.1531e-06,8.1081e-07,3.89e-07,5.1115e-07,4.2023e-07},
        {0.00024747,0.00024747,0.00024747,0.00018741,0.00013205,8.6421e-05,5.9389e-05,3.4888e-05,2.2079e-05,1.5626e-05,9.6199e-06,7.1251e-06,4.501e-06,2.494e-06,1.6055e-06,1.2889e-06,6.5396e-07,6.2963e-07,6.7431e-07},
        {0.00035526,0.00035526,0.00035526,0.00027625,0.00020011,0.00013324,9.1191e-05,5.9959e-05,3.8511e-05,2.2793e-05,1.6983e-05,1.1377e-05,7.3531e-06,4.2286e-06,2.5571e-06,1.9749e-06,1.6182e-06,9.4436e-07,1.3488e-06},
        {0.00052051,0.00052051,0.00052051,0.00039445,0.00028997,0.00020393,0.00013684,8.5134e-05,5.7565e-05,3.9087e-05,2.5041e-05,1.7784e-05,1.1357e-05,7.0355e-06,5.7461e-06,3.0124e-06,2.7606e-06,1.4614e-06,2.1379e-06},
        {0.00080001,0.00080001,0.00080001,0.00062118,0.00045826,0.00032672,0.00021967,0.00013586,0.00010235,6.6566e-05,4.4084e-05,2.8449e-05,2.0294e-05,1.3208e-05,1.1216e-05,6.5765e-06,4.5897e-06,3.5523e-06,4.5929e-06},
        {0.0012601,0.0012601,0.0012601,0.00098071,0.00071092,0.00053704,0.00036718,0.00023972,0.00017184,0.00011076,7.5275e-05,5.229e-05,3.8716e-05,2.6961e-05,2.0404e-05,1.3035e-05,9.5868e-06,8.4103e-06,9.7575e-06},
        {0.0020605,0.0020605,0.0020605,0.0016569,0.0012302,0.00086144,0.00063962,0.00043036,0.00030516,0.00021157,0.00014972,0.00010814,7.8289e-05,5.6678e-05,3.9213e-05,3.1662e-05,2.462e-05,2.0766e-05,2.4685e-05},
        {0.0032589,0.0032589,0.0032589,0.0026595,0.0019446,0.0014521,0.0010319,0.00070399,0.00051806,0.00037891,0.00028175,0.00020688,0.00015597,0.00012755,9.2153e-05,7.5553e-05,7.445e-05,5.3597e-05,5.7421e-05},
        {0.0053277,0.0053277,0.0053277,0.0039754,0.0030524,0.0022568,0.0017004,0.0012066,0.00089636,0.00072396,0.0005396,0.00042879,0.00033598,0.00028995,0.00025146,0.00020121,0.000181,0.00015669,0.00014439},
        {0.0084167,0.0084167,0.0084167,0.006023,0.0046633,0.0038726,0.0030495,0.0025353,0.002269,0.0018196,0.0017762,0.0015365,0.0012093,0.0011382,0.0010894,0.00099591,0.00090157,0.00076261,0.00077165}
    };
      
    //store arrays
    //flux_factor: to account for increased flux at 14 TeV vs 7-8 TeV (approximate)
    //*divide* lumiscale params by this since increased flux -> faster darkening
    float flux_factor = 1.2;
    for(int j = 0; j < maxEta; j++){
      for(int i = 0; i < maxLay; i++){
        lumiscale[j][i] = _lumiscale[j][i]/flux_factor;
        towerDose[j][i] = _towerDose[j][i];
      }
    }
  
  }
  
  ~HEDarkening() { }
  
  float degradation(float intlumi, int ieta, int lay) {
    //no lumi, no darkening
    if(intlumi <= 0) return 1.;
    
    //shift ieta tower index
    ieta -= ieta_shift;
    
    //if outside eta range, no darkening
    if(ieta < 0 || ieta >= maxEta) return 1.;
    
    //shift layer index by 1 to act as array index
    lay += 1;
    
    //if outside layer range, no darkening
    if(lay < 0 || lay >= maxLay) return 1.;
    
    if(mode==1 && ieta>=finger_ieta){
      //should fingers have an intlumi offset to account for installation?
      
      //attenuation
      float thedose = towerDose[ieta][lay]*intlumi; //convert dose from fb-1 to MRad
      float part1 = 1;
      //if(thedose>0) part1 = exp(-thedose/finger_scale);
      //if(thedose>0) part1 = exp(-thedose/0.044);

      //dose rate effect
      //from Biagtan, NIM B108 (1996) 125-128
	  //and Wick, NIM B61 (1991) 472-486
      float doserate = towerDose[ieta][lay]*inst_lumi; //use inst_lumi to get dose in MRad/hr
      float f0 = 0.043; //param for f(D)
      //float f0 = 0.073; //param for f(D)
      float f = fmax(0.0,fmin(1.0 - exp(-f0*thedose),1.0)); //fraction of fluor destroyed, dependent on dose (poisson model)
      float g = 0.024; //scaling factor for z0 vs dose rate
      float z0 = sqrt(g/doserate); //diffusion thickness
      float d = 4; //total thickness in mm
      float a0 = 0.028; //param for a(D)
      //float a0 = 0.00062; //param for a(D)
      float a = fmax(0.0,fmin(1.0 - a0*thedose,1.0)); //light yield asymptote
      float part2 = 1;
      if(doserate>0) part2 = fmax(a - f*2*z0/d, a - f);
	  //cout << "a = " << a << ", f = " << f << ", z0 = " << z0 << endl;
	  //cout << "D = " << thedose << ", R = " << doserate << ", L = " << part2 << endl;
	  
      //multiply part1 and part2 to get final darkening factor (relative light yield)
      return part1*part2;
    }
	else if (mode==2){
	  //use power law to get lumiscale from towerdose
	  if(towerDose[ieta][lay]==0) return 0;
	  double L0 = 2.6*pow(towerDose[ieta][lay],-0.583);
	  return (exp(-intlumi/L0));
	}
    else {
      //return darkening factor
      return (exp(-intlumi/lumiscale[ieta][lay]));
    }
  }

  int mode;
  int ieta_shift;
  int finger_ieta;
  float finger_scale, inst_lumi;
  float lumiscale[maxEta][maxLay];
  float towerDose[maxEta][maxLay];

};

#endif