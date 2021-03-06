#! /usr/bin/env python
import os
import sys
from math import *
from numpy import ndarray
from array import array
import ROOT
import optparse

parser = optparse.OptionParser("usage example: ./mapMaker.py -d HB -e 7TeV -m relyield", description="This is a script to produce dose rate maps vs layer and tower for HE and HB.")

parser.add_option ('-d','--det', type='string', dest="det",
                   help="HCAL subdetector: HE or HB")
parser.add_option ('-e','--energy', type='string', dest="energy",
                   help="LHC beam energy: 4TeV or 7TeV")
parser.add_option ('-m','--map', type='string', dest="map",
                   help="type of map: dose,  relyield, doserate, pe")

options, args = parser.parse_args()
if len(args) < 0:
    parser.print_usage()
    exit(1)

# Function: calculate dose per bin [Mrad]
def theDose(dr):
    lhc_runs = [5.6, 23.3, 4.1, 30, 65, 65, 100, 100, 107, 4000] #fbinv (last 2500)
    lhc_doserates = [.005, .013, .009, .026, .043, .043, .05, .05, .05, .15] #fbinv/hr
    totaldose = 0.
    for i in range(0,len(lhc_runs)):
        totaldose += lhc_runs[i]*dr # pre-LS3 and post-LS3 all included
    #totaldose += lhc_runs[9]*dr  #post-LS3 only
    return(totaldose)


# Function: calculate dose rate per bin after LS3 [krad/hr]
def dRate(dr):
    #dr is in Mrad/fb-1; rad/hr is needed
    lhc_runs = [5.6, 23.3, 4.1, 30, 65, 65, 100, 100, 107, 2500] #fbinv
    lhc_doserates = [.005, .013, .009, .027, .043, .043, .05, .05, .05, .15] #fbinv/hr
    #x = dr*lhc_doserates[len(lhc_doserates)-1]*1000000. # post-LS3 only
    x = dr*lhc_doserates[3]*1000000. # 2016 only
    #print lhc_doserates[3],x
    return(x)


# Function: calculate dose constant [Mrad] from dose rate per bin [Mrad/fb-1]
def doseconstant(dr):
    a = 1.57
    b = 0.538
    return a*pow(dr,b)#*2 #for HE #units: Mrad


# Function: calculate relative light yield per bin after, for example, 3000 fb-1
def relLightYield(dr):
    #doseconstant = a*doserate^b
    #relLightYield = e^(-luminosity/doseconstant)
    #3000 fb-1 in (1/3)*10 years (87600/3 = 29200 hours)
    #dr is in Mrad/fb-1; krad/hr is needed
    lhc_runs = [5.6, 23.3, 4.1, 30, 65, 65, 100, 100, 107, 2500] #fbinv
     #          11=5.6 2012=23.3 2015=30 2016=30  17-18=65 21-22-23=100, post LS3=2500
    lhc_doserates = [.005, .013, .009, .026, .043, .043, .05, .05, .05, .15] #fbinv/hr
    if (len(lhc_runs) != len(lhc_doserates)):
        print "ERROR: vector of LHC runs and LHC doserates have different sizes!"
        exit(1)
    expo=0.
    lumi_tot = 0.

    # calculate exponent
    for i in range(0,len(lhc_runs)):
        expo-=lhc_runs[i]*dr/doseconstant(1000.*dr*lhc_doserates[i])
        lumi_tot += lhc_runs[i]
    #expo-=lhc_runs[9]*dr/doseconstant(1000.*dr*lhc_doserates[9]) #post-LS3 only

    x=expo
    return exp(x)



def whatsMyEta(theDet, theBin):
    tower_etas_HE = [1.305,1.392,1.479,1.566,1.653,1.740,1.830,1.930,2.043,2.172,2.322,2.500,2.650,2.868,3.000]
    tower_etas_HB = [0.087*0.5, 0.087*1.5, 0.087*2.5, 0.087*3.5, 0.087*4.5, 0.087*5.5, 0.087*6.5, 0.087*7.5, 0.087*8.5, 0.087*9.5, 0.087*10.5, 0.087*11.5, 0.087*12.5, 0.087*13.5, 0.087*14.5, 0.087*15.5]
    myEta = -999.
    if theDet == "HE":
        print "Finding HE eta"
        myEta = (tower_etas_HE[theBin-1] + tower_etas_HE[theBin])/2.0
        print "Eta =",myEta
        return myEta
    elif theDet == "HB":
        print "Finding HB eta"
        myEta = tower_etas_HB[theBin-1]
        print "Eta =",myEta
        return myEta
    else:
        print "Not a valid subdetector!"

def whatsMySine(eta):
    theta = 2.*atan(exp(-1.*eta))
    return sin(theta)

nX = 0
xLow = 0
xHigh = 0
nY = 0
yLow = 0
yHigh = 0

if options.det == "HE":
    nX = 18
    nY = 14
#    yLow = 16
#    yHigh = 30
    yLow = -29
    yHigh = -15
    xLow = 0
    xHigh = 18
elif options.det == "HB":
    nY = 17
    nX = 16
    xLow = 1
    xHigh = 17
    yLow = 0
    yHigh = 17
else:
    print "ERROR - detector name not recognized"

xLow -=0.5
xHigh -=0.5
yLow -=0.5
yHigh -=0.5

###Output root file###
#ROOT.gStyle.SetPaintTextFormat("4.3f"); #useless; set in rootlogon.C instead
outputFileString = options.energy + "_" + options.det + "_4500fb_" + options.map + "Map" #orig
outputFileName = outputFileString + ".root"
outputFile = ROOT.TFile(outputFileName, 'recreate')


###Initialization###
cname = "DoseMapCanvas"
mapCanvas = ROOT.TCanvas(cname, cname, 800, 500)
hname = ""
if (options.map == "dose"):
    hname = "FLUKA Dose Map (" + options.det + ")"
elif (options.map == "relyield"):
    hname = "Relative Signal Map (" + options.det + ")"
elif (options.map == "doserate"):
    hname = "Dose Rate Map (" + options.det + ")"
elif (options.map == "pe"):
    hname = "Rel PE S/N Map (" + options.det + ")"
else:
   print "Map option not recognized!"
   exit(1)

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
    mapHisto.GetZaxis().SetTitle("Dose (FLUKA) [Mrad]")
    mapHisto.GetZaxis().SetRangeUser(0.000000001, 0.1) #0.001, 100 for dose in Mrad
elif options.map == "relyield":
    mapHisto.GetZaxis().SetTitle("Relative signal (doserate model)")# after 3000 fb-1
    mapHisto.SetTitleSize(0.042,"Z")
    mapHisto.GetZaxis().SetRangeUser(0., 2.5)
elif options.map == "doserate":
    #N.B.: plotting doserate map for LS3
    mapHisto.GetZaxis().SetTitle("Dose rate [rad/hr]")
    mapHisto.GetZaxis().SetRangeUser(0.00001, 10.) #0.000000001, 0.1
elif options.map == "pe":
    mapHisto.GetZaxis().SetTitle("PE yield (S/N)")
    mapHisto.GetZaxis().SetRangeUser(0., 3.)

###Input file###
inputPrefix = "/afs/cern.ch/user/f/friccita/HCAL/RadDam/MarekMaps/"
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
                mapHisto.SetBinContent(columncounter, nY-rowcounter+1, theDose(dose))
                #mapHisto.SetBinContent(columncounter, nY-rowcounter+1, dose)
                #if columncounter == 2 or columncounter == 8:
                #    print "Layer",columncounter-1,", Tower",rowcounter-1,", dose",dose,"Mrad/fb-1"
            elif options.det == "HB":
                mapHisto.SetBinContent(rowcounter, columncounter, theDose(dose))
                #mapHisto.SetBinContent(rowcounter, columncounter, dose)
                #if columncounter == 2 or columncounter == 8:
                #    print "Layer",columncounter-1,", Tower",rowcounter-1,", dose",dose,"Mrad/fb-1"
        elif options.map == "relyield":
            if options.det == "HE":
                if dose == 0:
                    mapHisto.SetBinContent(columncounter, nY-rowcounter+1, dose)
                else:
                    if columncounter == 1: #nofilter
                        mapHisto.SetBinContent(columncounter, nY-rowcounter+1, 6.*relLightYield(dose))
                    else: #nofilter
                        mapHisto.SetBinContent(columncounter, nY-rowcounter+1, 2.5*relLightYield(dose))
                    #mapHisto.SetBinContent(columncounter, nY-rowcounter+1, relLightYield(dose)) ##orig
            elif options.det == "HB":
                if dose == 0:
                    mapHisto.SetBinContent(rowcounter, columncounter, dose)
                else:
                    if columncounter == 1: # nofilter
                        mapHisto.SetBinContent(rowcounter, columncounter, 6.*relLightYield(dose))
                    else: #nofilter
                        mapHisto.SetBinContent(rowcounter, columncounter, 2.5*relLightYield(dose))
                    #mapHisto.SetBinContent(rowcounter, columncounter, relLightYield(dose))  ## orig
        elif options.map == "doserate":
            if options.det == "HE":
                mapHisto.SetBinContent(columncounter, nY-rowcounter+1, dRate(dose))
                if columncounter == 2 or columncounter == 8:
                    print "Layer",columncounter-1,", Tower",rowcounter-1,", doserate",dRate(dose),"rad/hr"
            elif options.det == "HB":
                mapHisto.SetBinContent(rowcounter, columncounter, dRate(dose))
                if columncounter == 2 or columncounter == 8:
                    print "Layer",columncounter-1,", Tower",rowcounter-1,", doserate",dRate(dose),"rad/hr"
        elif options.map == "pe":
            if options.det == "HE":
                if dose == 0:
                    mapHisto.SetBinContent(columncounter, nY-rowcounter+1, dose)
                else:
                    eta = whatsMyEta("HE",rowcounter)
                    if columncounter == 1: #nofilter
                        #mapHisto.SetBinContent(columncounter, nY-rowcounter+1, 9./whatsMySine(eta))
                        mapHisto.SetBinContent(columncounter, nY-rowcounter+1, 2.4*relLightYield(dose)*9./(whatsMySine(eta)*6.2))
                    else: #nofilter
                        #mapHisto.SetBinContent(columncounter, nY-rowcounter+1, 9./whatsMySine(eta))
                        mapHisto.SetBinContent(columncounter, nY-rowcounter+1, relLightYield(dose)*9./(whatsMySine(eta)*6.2))

            elif options.det == "HB":
                if dose == 0:
                    mapHisto.SetBinContent(rowcounter, columncounter, dose)
                else:
                    eta = whatsMyEta("HB",rowcounter)
                    if columncounter == 1: # nofilter
                        #mapHisto.SetBinContent(rowcounter, columncounter, 9./whatsMySine(eta))
                        mapHisto.SetBinContent(rowcounter, columncounter, 2.4*relLightYield(dose)*9./(whatsMySine(eta)*6.2))
                    else: # nofilter
                        #mapHisto.SetBinContent(rowcounter, columncounter, 9./whatsMySine(eta))
                        mapHisto.SetBinContent(rowcounter, columncounter, relLightYield(dose)*9./(whatsMySine(eta)*6.2))
                


inputFile.close()

mapCanvas.cd()
if (options.map == "doserate"):
    mapCanvas.SetLogz()

mapHisto.Draw("colz text")
mapCanvas.SetRightMargin(0.20)
mapCanvas.Write()
mapCanvas.Print(outputFileString+".pdf",".pdf")
outputFile.Write()
outputFile.Close()
