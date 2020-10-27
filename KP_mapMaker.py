#! /usr/bin/env python
import os
import sys
from math import *
from numpy import ndarray
from array import array
#from ROOT import *
import ROOT
import optparse

parser = optparse.OptionParser("usage: %prog [options]", description="This is a script to produce dose rate maps vs layer and tower for HE and HB.")

parser.add_option ('-d','--det', type='string', dest="det",
                   help="HCAL subdetector: HE or HB")
parser.add_option ('-e','--energy', type='string', dest="energy",
                   help="LHC beam energy: 4TeV or 7TeV")
parser.add_option ('-m','--map', type='string', dest="map",
                   help="type of map: dose or relyield")

options, args = parser.parse_args()
if len(args) < 0:
    parser.print_usage()
    exit(1)

nX = 0
xLow = 0
xHigh = 0
nY = 0
yLow = 0
yHigh = 0
nEtaBins = 0
nLayers = 0

if options.det == "HE":
    nX = 18 #layers ...19?
    nY = 14 #towers
    nEtaBins = nY
    nLayers = nX
#    yLow = 16
#    yHigh = 30
    yLow = -29
    yHigh = -15
    xLow = 0
    xHigh = 18
elif options.det == "HB":
    nY = 17 #layers
    nX = 16 #towers
    nEtaBins = nX
    nLayers = nY
    xLow = 1
    xHigh = 17
    yLow = 0
    yHigh = 17
else:
    print "ERROR - detector name not recognized"

print "nLayers = " + str(nLayers) + " and nEta = " + str(nEtaBins)

xLow -=0.5
xHigh -=0.5
yLow -=0.5
yHigh -=0.5

flux_factor = 1.2
#FOR HE
lumiscale_HE = [
    [1194.9/flux_factor, 1194.9/flux_factor, 1194.9/flux_factor, 1651.5/flux_factor, 2282.7/flux_factor, 3155.2/flux_factor, 4361.0/flux_factor, 6027.8/flux_factor, 8331.5/flux_factor, 11515.7/flux_factor, 15916.8/flux_factor, 22000.0/flux_factor, 30408.2/flux_factor, 42029.8/flux_factor, 58093.1/flux_factor, 80295.6/flux_factor, 110983.5/flux_factor, 153400.1/flux_factor, 212027.7/flux_factor],
    [952.8/flux_factor, 952.8/flux_factor, 952.8/flux_factor, 1293.9/flux_factor, 1757.1/flux_factor, 2386.1/flux_factor, 3240.3/flux_factor, 4400.3/flux_factor, 5975.4/flux_factor, 8114.5/flux_factor, 11019.3/flux_factor, 14963.9/flux_factor, 20320.6/flux_factor, 27594.9/flux_factor, 37473.2/flux_factor, 50887.7/flux_factor, 69104.3/flux_factor, 93841.9/flux_factor, 127435.0/flux_factor],
    [759.8/flux_factor, 759.8/flux_factor, 759.8/flux_factor, 1013.8/flux_factor, 1352.5/flux_factor, 1804.5/flux_factor, 2407.6/flux_factor, 3212.2/flux_factor, 4285.7/flux_factor, 5717.9/flux_factor, 7628.7/flux_factor, 10178.1/flux_factor, 13579.5/flux_factor, 18117.6/flux_factor, 24172.3/flux_factor, 32250.4/flux_factor, 43028.0/flux_factor, 57407.4/flux_factor, 76592.2/flux_factor],
    [605.9/flux_factor, 605.9/flux_factor, 605.9/flux_factor, 794.2/flux_factor, 1041.1/flux_factor, 1364.7/flux_factor, 1788.9/flux_factor, 2344.9/flux_factor, 3073.7/flux_factor, 4029.1/flux_factor, 5281.4/flux_factor, 6922.9/flux_factor, 9074.7/flux_factor, 11895.2/flux_factor, 15592.4/flux_factor, 20438.8/flux_factor, 26791.5/flux_factor, 35118.8/flux_factor, 46034.2/flux_factor],
    [483.2/flux_factor, 483.2/flux_factor, 483.2/flux_factor, 622.3/flux_factor, 801.4/flux_factor, 1032.1/flux_factor, 1329.2/flux_factor, 1711.8/flux_factor, 2204.5/flux_factor, 2839.1/flux_factor, 3656.3/flux_factor, 4708.8/flux_factor, 6064.3/flux_factor, 7809.9/flux_factor, 10058.0/flux_factor, 12953.2/flux_factor, 16681.8/flux_factor, 21483.8/flux_factor, 27667.9/flux_factor],
    [385.3/flux_factor, 385.3/flux_factor, 385.3/flux_factor, 487.5/flux_factor, 616.9/flux_factor, 780.5/flux_factor, 987.6/flux_factor, 1249.6/flux_factor, 1581.1/flux_factor, 2000.6/flux_factor, 2531.3/flux_factor, 3202.8/flux_factor, 4052.5/flux_factor, 5127.6/flux_factor, 6487.9/flux_factor, 8209.2/flux_factor, 10387.0/flux_factor, 13142.6/flux_factor, 16629.3/flux_factor],
    [307.3/flux_factor, 307.3/flux_factor, 307.3/flux_factor, 382.0/flux_factor, 474.8/flux_factor, 590.3/flux_factor, 733.8/flux_factor, 912.2/flux_factor, 1134.0/flux_factor, 1409.7/flux_factor, 1752.4/flux_factor, 2178.5/flux_factor, 2708.1/flux_factor, 3366.6/flux_factor, 4185.1/flux_factor, 5202.6/flux_factor, 6467.5/flux_factor, 8039.9/flux_factor, 9994.7/flux_factor],
    [245.0/flux_factor, 245.0/flux_factor, 245.0/flux_factor, 299.3/flux_factor, 365.5/flux_factor, 446.4/flux_factor, 545.2/flux_factor, 665.9/flux_factor, 813.3/flux_factor, 993.3/flux_factor, 1213.2/flux_factor, 1481.8/flux_factor, 1809.7/flux_factor, 2210.3/flux_factor, 2699.6/flux_factor, 3297.2/flux_factor, 4027.0/flux_factor, 4918.4/flux_factor, 6007.1/flux_factor],
    [195.4/flux_factor, 195.4/flux_factor, 195.4/flux_factor, 234.5/flux_factor, 281.3/flux_factor, 337.6/flux_factor, 405.1/flux_factor, 486.1/flux_factor, 583.3/flux_factor, 700.0/flux_factor, 839.9/flux_factor, 1007.9/flux_factor, 1209.4/flux_factor, 1451.2/flux_factor, 1741.4/flux_factor, 2089.6/flux_factor, 2507.4/flux_factor, 3008.8/flux_factor, 3610.5/flux_factor],
    [155.8/flux_factor, 155.8/flux_factor, 155.8/flux_factor, 183.7/flux_factor, 216.6/flux_factor, 255.3/flux_factor, 301.0/flux_factor, 354.9/flux_factor, 418.4/flux_factor, 493.2/flux_factor, 581.5/flux_factor, 685.5/flux_factor, 808.2/flux_factor, 952.8/flux_factor, 1123.3/flux_factor, 1324.3/flux_factor, 1561.3/flux_factor, 1840.6/flux_factor, 2170.0/flux_factor],
    [124.3/flux_factor, 124.3/flux_factor, 124.3/flux_factor, 143.9/flux_factor, 166.7/flux_factor, 193.1/flux_factor, 223.6/flux_factor, 259.0/flux_factor, 300.1/flux_factor, 347.5/flux_factor, 402.6/flux_factor, 466.3/flux_factor, 540.1/flux_factor, 625.6/flux_factor, 724.6/flux_factor, 839.3/flux_factor, 972.1/flux_factor, 1126.0/flux_factor, 1304.2/flux_factor],
    [99.1/flux_factor, 99.1/flux_factor, 99.1/flux_factor, 112.8/flux_factor, 128.3/flux_factor, 146.0/flux_factor, 166.2/flux_factor, 189.1/flux_factor, 215.2/flux_factor, 244.9/flux_factor, 278.7/flux_factor, 317.2/flux_factor, 360.9/flux_factor, 410.7/flux_factor, 467.4/flux_factor, 531.9/flux_factor, 605.3/flux_factor, 688.8/flux_factor, 783.9/flux_factor],
    [79.0/flux_factor, 79.0/flux_factor, 79.0/flux_factor, 88.3/flux_factor, 98.8/flux_factor, 110.4/flux_factor, 123.5/flux_factor, 138.0/flux_factor, 154.3/flux_factor, 172.6/flux_factor, 192.9/flux_factor, 215.7/flux_factor, 241.2/flux_factor, 269.7/flux_factor, 301.5/flux_factor, 337.1/flux_factor, 376.9/flux_factor, 421.4/flux_factor, 471.1/flux_factor],
    [63.0/flux_factor, 63.0/flux_factor, 63.0/flux_factor, 69.2/flux_factor, 76.0/flux_factor, 83.5/flux_factor, 91.7/flux_factor, 100.8/flux_factor, 110.7/flux_factor, 121.6/flux_factor, 133.6/flux_factor, 146.7/flux_factor, 161.2/flux_factor, 177.0/flux_factor, 194.5/flux_factor, 213.6/flux_factor, 234.7/flux_factor, 257.8/flux_factor, 283.2/flux_factor]
]

#FOR HB
lumiscale_HB = [
      [887.1,887.1,1199.3,1314.0,1615.8,1954.0,2504.1,2561.7,3249.6,4553.7,5626.1,5725.6,6777.4,8269.8,10061.8,15253.3,22200.9],
      [990.8,990.8,1140.8,1325.4,1676.9,2036.6,2900.5,2789.1,3460.2,4405.9,4184.0,5794.6,6157.4,7646.0,11116.1,18413.9,26813.3],
      [971.2,971.2,1244.3,1456.2,1760.1,2299.1,2603.2,3012.3,3933.9,4787.3,4503.9,6624.3,7059.4,9369.5,12038.0,20048.8,23541.3],
      [877.3,877.3,1145.2,1322.5,1604.9,1924.0,2893.6,2827.4,4085.0,5320.2,4740.6,5693.5,5715.1,7373.4,8305.1,16079.9,21702.3],
      [919.8,919.8,1223.2,1376.9,1742.6,1964.7,2494.7,3335.6,4520.6,4869.5,4895.6,5740.4,7089.8,8765.9,10045.7,17408.0,24726.7],
      [901.1,901.1,1114.3,1391.2,1733.2,2210.7,2733.8,3399.4,3715.2,3626.3,3371.3,4653.8,5911.6,7204.2,7584.7,4760.1,11156.8],
      [904.0,904.0,1112.9,1351.9,1722.3,2008.2,2709.8,3101.9,3470.5,4679.0,5843.6,6343.8,7883.3,11266.8,16607.2,10882.3,25428.4],
      [930.7,930.7,1225.3,1341.9,1744.0,2253.8,2805.1,3329.9,3665.6,5179.6,5677.8,5753.0,5662.3,9516.5,10769.4,13892.9,16661.1],
      [953.2,953.2,1240.4,1487.3,1719.8,1930.6,2595.7,3172.5,3881.0,5247.5,4934.0,6576.4,6353.7,9259.2,12264.5,13261.8,12222.6],
      [877.4,877.4,1114.1,1346.0,1604.9,1997.6,2708.9,3247.9,3704.4,4568.2,4984.4,7000.8,7896.7,7970.0,12555.6,10062.1,18386.8],
      [876.2,876.2,1127.1,1336.0,1753.1,1944.4,2641.1,3445.1,3810.2,4033.0,5301.2,5170.7,6062.3,9815.8,12854.2,14297.3,20692.1],
      [841.2,841.2,1051.1,1229.5,1576.5,1983.2,2282.2,2981.2,3271.7,4417.1,3765.2,4491.8,4626.6,7173.2,12953.0,7861.2,19338.6],
      [848.2,848.2,1072.8,1228.5,1497.9,1876.1,2279.7,2744.2,3325.9,4021.8,4081.1,3750.6,4584.2,6170.8,9020.5,12058.8,13492.2],
      [780.3,780.3,977.6,1103.2,1323.2,1548.3,1970.1,2217.5,2761.0,3049.0,2913.0,3832.3,4268.6,5242.1,6432.8,5999.5,6973.9],
      [515.2,515.2,662.1,836.5,1110.4,1214.4,1664.4,1919.0,2341.0,2405.2,2647.5,2593.8,2586.3,2814.4,2826.8,0.0,0.0],
      [409.3,409.3,489.2,700.3,960.2,1103.5,909.8,934.6,1148.6,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0]
]


def relLightYield(eta, layer):
    #doseconstant = a*doserate^b
    #relLightYield = e^(-luminosity/doseconstant)
    #3000 fb-1 in (1/3)*10 years (87600/3 = 29200 hours)
    #dr is in Mrad/fb-1; krad/hr is needed
    intlumi = 3000 # fb^-1
    if options.det == "HE":
        if lumiscale_HE[eta][layer] == 0:
            x = -intlumi
        else:
            x = -intlumi/lumiscale_HE[eta][layer]
    elif options.det == "HB":
        if lumiscale_HB[eta][layer] == 0:
            x = -intlumi
        else:
            x = -intlumi/lumiscale_HB[eta][layer]
        
    else:
        print "ERROR (relLightYield): subdetector not recognized"
    return exp(x)
    

###Output root file###
ROOT.gStyle.SetPaintTextFormat("4.2f"); #useless; set in rootlogon.C instead
outputFileName = "KP_" + options.energy + "_" + options.det + "_3000fb_" + options.map + "Map.root"
outputFile = ROOT.TFile(outputFileName, 'recreate')


###Initialization###
cname = "DoseMapCanvas"
mapCanvas = ROOT.TCanvas(cname, cname, 800, 500)
hname = ""
if (options.map == "dose"):
    hname = "FLUKA Dose Map (" + options.det + ")"
elif (options.map == "relyield"):
    hname = "Relative Signal Map (" + options.det + ")"

tower_etas_HE = [1.305,1.392,1.479,1.566,1.653,1.740,1.830,1.930,2.043,2.172,2.322,2.500,2.650,2.868,3.000]
tower_etas_HB = [0.087*0.5, 0.087*1.5, 0.087*2.5, 0.087*3.5, 0.087*4.5, 0.087*5.5, 0.087*6.5, 0.087*7.5, 0.087*8.5, 0.087*9.5, 0.087*10.5, 0.087*11.5, 0.087*12.5, 0.087*13.5, 0.087*14.5, 0.087*15.5]

mapHisto = ROOT.TH2F(hname, hname, nX, xLow, xHigh, nY, yLow, yHigh)
#input text file rows -> y axis, columns -> x axis, values -> z axis
if options.det == "HE":
    mapHisto.GetXaxis().SetTitle("Layer")
    mapHisto.GetYaxis().SetTitle("Tower")
    for j in range(0, nY):
        if j == nY:
            break
        else:
            tempo = (tower_etas_HE[j]+tower_etas_HE[j+1])/2.0
            tempo2 = float('%.2f' % tempo)
            binname = str(tempo2)
            ybin = mapHisto.GetYaxis().FindBin(-(j+16));
            mapHisto.GetYaxis().SetCanExtend(True)
            mapHisto.GetYaxis().SetBinLabel(ybin, binname);
elif options.det == "HB":
    mapHisto.GetXaxis().SetTitle("Tower")
    mapHisto.GetYaxis().SetTitle("Layer")
    for j in range(0, nX):
        if j == nX:
            break
        else:
            tempo = tower_etas_HB[j]
            tempo2 = float('%.2f' % tempo)
            binname = str(tempo2)
            xbin = mapHisto.GetXaxis().FindBin(j+0.5);
            mapHisto.GetXaxis().SetCanExtend(True)
            mapHisto.GetXaxis().SetBinLabel(xbin, binname);

if options.map == "dose":
    mapHisto.GetZaxis().SetTitle("Dose (FLUKA) [Mrad/fb-1]")
    mapHisto.GetZaxis().SetRangeUser(0.000000001, 0.1)
elif options.map == "relyield":
    mapHisto.GetZaxis().SetTitle("Relative signal (2012 model) after 3000 fb-1")
    mapHisto.SetTitleSize(0.042,"Z")
    mapHisto.GetZaxis().SetRangeUser(0., 1.)



###Input file###
inputPrefix = "/afs/cern.ch/user/f/friccita/HCAL28/RadDam/MarekMaps/"
inputFileName = inputPrefix + "fluka_Mrad_fbinv_" + options.energy + "-" + options.det + ".txt"

inputFile = open(inputFileName, 'r')
rowcounter = 0
for line in inputFile:
    rowcounter += 1
    values = line.split()
    columncounter = 0
    for num in values:
        columncounter += 1
        dose = float(num)
        if options.map == "dose":
            if options.det == "HE":
                mapHisto.SetBinContent(columncounter, nY-rowcounter, dose)
            elif options.det == "HB":
                mapHisto.SetBinContent(rowcounter, columncounter, dose)
        elif options.map == "relyield":
            if options.det == "HE":
                if dose == 0:
                    mapHisto.SetBinContent(columncounter, nY-rowcounter+1, dose)
                else:
                    #print nY-rowcounter
                    #print columncounter-1
                    mapHisto.SetBinContent(columncounter, nY-rowcounter+1, relLightYield(rowcounter-1,columncounter-1)) #nY-rowcounter
            elif options.det == "HB":
                if dose == 0:
                    mapHisto.SetBinContent(rowcounter, columncounter, dose)
                else:
                    #print rowcounter-1
                    #print columncounter-1
                    if relLightYield(rowcounter-1, columncounter-1) == 0:
                        mapHisto.SetBinContent(rowcounter, columncounter, relLightYield(rowcounter-1, columncounter-1))
                    else:
                        mapHisto.SetBinContent(rowcounter, columncounter, relLightYield(rowcounter-1, columncounter-1))


inputFile.close()

mapCanvas.cd()
if (options.map == "dose"):
    mapCanvas.SetLogz()
mapHisto.Draw("colz text")
mapCanvas.SetRightMargin(0.20)
mapCanvas.Write()
outputFile.Write()
outputFile.Close()
