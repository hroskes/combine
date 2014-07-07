CWD=`pwd -P`
#source /afs/cern.ch/cms/caf/setup.sh
cd /scratch0/hep/hroskes/CMSSW_6_1_1/src/
export SCRAM_ARCH=slc5_amd64_gcc472
eval `scramv1 ru -sh`

extent=0
if [ $# -gt 0 ]
then
    extent=$1
fi

if [ $extent -ge 100 ]
then

    #compile python code

    scram b
fi

cd /scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards

if [ $extent -ge 90 ]
then

    #compile C++ code

    ./buildPackage.sh
fi
if [ $extent -ge 70 ]
then
    echo "cd /afs/cern.ch/user/h/hroskes/work/powheg/mHequalsmZ/combine/2dplots" > forsshtolxplus.txt
    if [ $extent -ge 80 ]
    then

        #Get the rates from the trees

        echo "root -l -b -q setRates.C+" >> forsshtolxplus.txt
        echo "scp -r /afs/cern.ch/work/h/hroskes/powheg/mHequalsmZ/combine/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards/SM_inputs_?TeV/ hroskes@hep.pha.jhu.edu:/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards" >> forsshtolxplus.txt

        #find the discriminant

        echo "root -l -b -q findDiscriminant.C+" >> forsshtolxplus.txt
    fi

    #make the templates

    echo "root -l -b -q make2dPlot.C+" >> forsshtolxplus.txt
    echo "scp -r templates2DHeshy_*TeV hroskes@hep.pha.jhu.edu:/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards" >> forsshtolxplus.txt
    if [ $extent -ge 77 ]
    then

        #Run prepareData

        echo "cd /afs/cern.ch/user/h/hroskes/work/powheg/mHequalsmZ/combine/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/AnalysisInputs" >> forsshtolxplus.txt
        echo "scp hroskes@hep.pha.jhu.edu:/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/AnalysisInputs/prepareDataFromUlascan.C ." >> forsshtolxplus.txt
        echo "scp hroskes@hep.pha.jhu.edu:/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/AnalysisInputs/Config.h ." >> forsshtolxplus.txt
        echo "root -l -b -q prepareDataFromUlascan.C+" >> forsshtolxplus.txt
        echo "cd ../CreateDatacards/CMSdata" >> forsshtolxplus.txt
        echo "scp -r *.root hroskes@hep.pha.jhu.edu:/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards/CMSdata" >> forsshtolxplus.txt
    fi
    cat forsshtolxplus.txt | ssh -Y hroskes@lxplus5.cern.ch 
fi


mkdir -p cards_HeshyExpected/HCG/91.2/
mkdir -p cards_HeshyExpected/HCG_XSxBR/91.2/

python make_prop_DCsandWSs.py -t "templates2DHeshy" -a "HeshyExpected" -b -m 1D -i SM_inputs_8TeV 2>&1 &&
python make_prop_DCsandWSs.py -t "templates2DHeshy" -a "HeshyExpected" -b -m 1D -i SM_inputs_7TeV 2>&1
if [ $? -ne 0 ]
then
    exit
fi

cp -r cards_HeshyExpected cards_HeshyObserved

cd cards_HeshyExpected/HCG/
combineCards.py ./91.2/hzz4l_2e2muS_7TeV.txt ./91.2/hzz4l_4muS_7TeV.txt ./91.2/hzz4l_4eS_7TeV.txt ./91.2/hzz4l_2e2muS_8TeV.txt ./91.2/hzz4l_4muS_8TeV.txt ./91.2/hzz4l_4eS_8TeV.txt > hzz4l_allS_78TeV.txt &&
text2workspace.py -m 91.2 hzz4l_allS_78TeV.txt -P HiggsAnalysis.CombinedLimit.SpinZeroStructure:spinZeroHiggs -o hzz4l_allS_fZ4l_78TeV.root --PO='' &&
combine -M MultiDimFit hzz4l_allS_fZ4l_78TeV.root --algo=grid --points 200 -S 1 -m 91.2 -n obs_1D_fL1_PMF -v 3 -t -1 --expectSignal=1 --saveNLL --setPhysicsModelParameters r=1,CMS_zz4l_fai1=0 | tee log_exp_1D_fL1_PMF.log
cd - > /dev/null

cd cards_HeshyObserved/HCG/
combineCards.py ./91.2/hzz4l_2e2muS_7TeV.txt ./91.2/hzz4l_4muS_7TeV.txt ./91.2/hzz4l_4eS_7TeV.txt ./91.2/hzz4l_2e2muS_8TeV.txt ./91.2/hzz4l_4muS_8TeV.txt ./91.2/hzz4l_4eS_8TeV.txt > hzz4l_allS_78TeV.txt &&
text2workspace.py -m 91.2 hzz4l_allS_78TeV.txt -P HiggsAnalysis.CombinedLimit.SpinZeroStructure:spinZeroHiggs -o hzz4l_allS_fZ4l_78TeV.root --PO='' &&
combine -M MultiDimFit hzz4l_allS_fZ4l_78TeV.root --algo=grid --points 200 -S 1 -m 91.2 -n obs_1D_fL1_PMF -v 3 --saveNLL --setPhysicsModelParameters r=1,CMS_zz4l_fai1=0 | tee log_obs_1D_fL1_PMF.log
cd - > /dev/null
