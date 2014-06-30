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
    scram b
fi

cd /scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards

if [ $extent -ge 90 ]
then
    ./buildPackage.sh
fi
if [ $extent -ge 70 ]
then
    echo "cd /afs/cern.ch/user/h/hroskes/work/powheg/mHequalsmZ/combine/2dplots" > forsshtolxplus.txt
    if [ $extent -ge 80 ]
    then
        echo "root -l -b -q getSumOfWeights.C+" >> forsshtolxplus.txt
        echo "root -l -b -q rateQQZZ.C+" >> forsshtolxplus.txt
        echo "scp -r /afs/cern.ch/work/h/hroskes/powheg/mHequalsmZ/combine/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards/SM_inputs_?TeV/ hroskes@hep.pha.jhu.edu:/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards" >> forsshtolxplus.txt
        echo "root -l -b -q findDiscriminant.C+" >> forsshtolxplus.txt
    fi
    echo "root -l -b -q make2dPlot.C+" >> forsshtolxplus.txt
    echo "scp -r templates2DHeshy_*TeV hroskes@hep.pha.jhu.edu:/scratch0/hep/hroskes/CMSSW_6_1_1/src/HiggsAnalysis/HZZ4l_Combination/CreateDatacards" >> forsshtolxplus.txt
    cat forsshtolxplus.txt | ssh -Y hroskes@lxplus5.cern.ch 
fi

python make_prop_DCsandWSs.py -t "templates2DHeshy" -a "HeshyTest" -b -m 1D -i SM_inputs_8TeV 2>&1 &&
python make_prop_DCsandWSs.py -t "templates2DHeshy" -a "HeshyTest" -b -m 1D -i SM_inputs_7TeV 2>&1 &&
cd cards_HeshyTest/HCG/ &&
combineCards.py ./91.2/hzz4l_2e2muS_7TeV.txt ./91.2/hzz4l_4muS_7TeV.txt ./91.2/hzz4l_4eS_7TeV.txt ./91.2/hzz4l_2e2muS_8TeV.txt ./91.2/hzz4l_4muS_8TeV.txt ./91.2/hzz4l_4eS_8TeV.txt > hzz4l_allS_78TeV.txt &&
text2workspace.py -m 91.2 hzz4l_allS_78TeV.txt -P HiggsAnalysis.CombinedLimit.SpinZeroStructure:spinZeroHiggs -o hzz4l_allS_fZ4l_78TeV.root --PO='' &&
combine -M MultiDimFit hzz4l_allS_fZ4l_78TeV.root --algo=grid --points 200 -S 1 -m 91.2 -n obs_1D_fL1_PMF -v 3 --saveNLL --setPhysicsModelParameters r=1,CMS_zz4l_fai1=0 | tee log_obs_1D_fL1_PMF.log
