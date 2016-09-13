# -*- coding: utf-8 -*-
"""
Created on Sat Jul 25 11:21:31 2015

@author: aschmitt
"""
 
from PyQt4 import QtGui, QtCore
import sys
import os
import inspect
import glob

import aview
import aviewwidget
import resultstat
import resparser

subfolder = "../stats"
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],subfolder)))
if cmd_subfolder not in sys.path:
    #print("inserting sys path : cmd_subfolder = {}".format(cmd_subfolder))
    sys.path.insert(0, cmd_subfolder)
     
import processAmazedOutputStats
 
class ObjectAViewGui(object):
    def __init__(self, parent=None):
        self.value = []
        self.parent = parent
 
 
class AViewGui(QtGui.QWidget):
    def __init__(self, parent=None, init_resdir="", init_refpath=""):
        super(AViewGui, self).__init__(parent)
        self.parent = parent
        wdg = self#QtGui.QWidget()
        layout = QtGui.QGridLayout(wdg)     
        
        
        layoutRow = 0
        separator_height = 20        
        separator_ncols = 5
        
        #Add the configuration separator
        self.lblInputSection = QtGui.QLabel('Load results', wdg) 
        self.lblInputSection.setAlignment(QtCore.Qt.AlignCenter)           
        self.lblInputSection.setFixedHeight(separator_height)
        self.lblInputSection.setProperty("coloredcell", True)
        layout.addWidget(self.lblInputSection, layoutRow, 0, 1, 1) 
        for i in range(1, separator_ncols):
            lbl = QtGui.QLabel('', wdg)
            if i==separator_ncols-1:
                lbl.setFixedWidth(100)
            lbl.setProperty("coloredcell", True)
            layout.addWidget(lbl, layoutRow, i, 1, 1)         
 
 
        #Add the result dir. setup ctrls
        layoutRow += 1
        self.lblResdir = QtGui.QLabel(' AMAZED Output Result Dir. ', wdg)
        layout.addWidget(self.lblResdir, layoutRow, 0, 1, 1)
        
        self.leResDir = QtGui.QLineEdit(wdg)
        self.leResDir.setFixedWidth(500) 
        layout.addWidget(self.leResDir, layoutRow, 1, 1, 10)
        
        self.btnBrowseResdir = QtGui.QPushButton(' Browse ', wdg)
        self.btnBrowseResdir.setToolTip('Browse to select the AMAZED output directory...')
        self.btnBrowseResdir.clicked.connect(self.bt_setResultDir)
        layout.addWidget(self.btnBrowseResdir, layoutRow, 2, 1, 1)
        
        self.btnBrowseResdirPrevious = QtGui.QPushButton(' < ', wdg)
        self.btnBrowseResdirPrevious.setToolTip('Back to the previous ResDir...')
        self.btnBrowseResdirPrevious.clicked.connect(self.bt_setResultDirPrevious)
        layout.addWidget(self.btnBrowseResdirPrevious, layoutRow, 3, 1, 1)
        
        #Add the failure threshold filter ctrls
        layoutRow += 1
        self.lblDiffThres = QtGui.QLabel(' Filter by z diff. threshold: ', wdg)
        layout.addWidget(self.lblDiffThres, layoutRow, 0, 1, 1)
        
        self.leDiffThres = QtGui.QLineEdit(wdg)
        self.leDiffThres.setToolTip('Enter threshold value to filter the dataset by the z error value...')
        self.leDiffThres.setFixedWidth(200) 
        layout.addWidget(self.leDiffThres, layoutRow, 1, 1, 10)
        
        #Add the spectrum filter ctrls
        layoutRow += 1
        self.lblSpcFilter = QtGui.QLabel(' Filter by spectrum name: ', wdg)
        layout.addWidget(self.lblSpcFilter, layoutRow, 0, 1, 1)
        
        self.leSpcFilter = QtGui.QLineEdit(wdg)
        self.leSpcFilter.setToolTip('Enter a tag to filter the dataset by the spectrum name...')
        self.leSpcFilter.setFixedWidth(300) 
        layout.addWidget(self.leSpcFilter, layoutRow, 1, 1, 10)
        
        #Add the show button
        layoutRow += 1
        self.btnLoad = QtGui.QPushButton('Load Result List', wdg)
        self.btnLoad.setFixedWidth(500)
        self.btnLoad.setFixedHeight(50)
        self.btnLoad.clicked.connect(self.bt_loadresults)
        layout.addWidget(self.btnLoad, layoutRow, 1, 1, 1)
        
        #Add the result list N
        layoutRow += 1
        self.lblResultsN = QtGui.QLabel('N Results loaded:', wdg)
        layout.addWidget(self.lblResultsN, layoutRow, 0, 1, 1) 
        self.leResultsN = QtGui.QLineEdit(wdg)
        self.leResultsN.setFixedWidth(100) 
        layout.addWidget(self.leResultsN, layoutRow, 1, 1, 10) 
        self.leResultsN.setEnabled(False)
        
        #Add the result list section separator
        layoutRow += 1
        self.lblResultListSection = QtGui.QLabel('Browse results', wdg) 
        self.lblResultListSection.setAlignment(QtCore.Qt.AlignCenter)           
        self.lblResultListSection.setFixedHeight(separator_height)
        self.lblResultListSection.setProperty("coloredcell", True)
        layout.addWidget(self.lblResultListSection, layoutRow, 0, 1, 1) 
        for i in range(1, separator_ncols):
            lbl = QtGui.QLabel('', wdg)
            if i==separator_ncols-1:
                lbl.setFixedWidth(100)
            lbl.setProperty("coloredcell", True)
            layout.addWidget(lbl, layoutRow, i, 1, 1)   
        
        #Add the result index
        layoutRow += 1
        self.lblResultIndex = QtGui.QLabel('Result #', wdg)
        layout.addWidget(self.lblResultIndex, layoutRow, 0, 1, 1) 
        self.leResultIndex = QtGui.QLineEdit(wdg)
        self.leResultIndex.setFixedWidth(100) 
        layout.addWidget(self.leResultIndex, layoutRow, 1, 1, 2) 

        self.btnSetResultPrevious = QtGui.QPushButton(' < ', wdg)
        self.btnSetResultPrevious.setToolTip('Got to the previous Result in the list...')
        self.btnSetResultPrevious.clicked.connect(self.bt_previousResultIndex)
        layout.addWidget(self.btnSetResultPrevious, layoutRow, 2, 1, 1)
        self.btnSetResultNext = QtGui.QPushButton(' > ', wdg)
        self.btnSetResultNext.setToolTip('Go to the next Result in the list...')
        self.btnSetResultNext.clicked.connect(self.bt_nextResultIndex)
        layout.addWidget(self.btnSetResultNext, layoutRow, 3, 1, 1)
        #Add the result name 
        layoutRow += 1
        self.lblResultName = QtGui.QLabel('        - name', wdg)
        layout.addWidget(self.lblResultName, layoutRow, 0, 1, 1) 
        self.leResultSpcName = QtGui.QLineEdit(wdg)
        self.leResultSpcName.setFixedWidth(500) 
        layout.addWidget(self.leResultSpcName, layoutRow, 1, 1, 2) 
        self.leResultSpcName.setEnabled(False)
        #Add the result zdiff
        layoutRow += 1
        self.lblResultZdiff = QtGui.QLabel('        - zdiff', wdg)
        layout.addWidget(self.lblResultZdiff, layoutRow, 0, 1, 1) 
        self.leResultSpcZdiff = QtGui.QLineEdit(wdg)
        self.leResultSpcZdiff.setFixedWidth(300) 
        layout.addWidget(self.leResultSpcZdiff, layoutRow, 1, 1, 2) 
        self.leResultSpcZdiff.setEnabled(False)
        #Add the result zcalc 
        layoutRow += 1
        self.lblResultZcalc = QtGui.QLabel('        - zcalc', wdg)
        layout.addWidget(self.lblResultZcalc, layoutRow, 0, 1, 1) 
        self.leResultSpcZcalc = QtGui.QLineEdit(wdg)
        self.leResultSpcZcalc.setFixedWidth(100) 
        layout.addWidget(self.leResultSpcZcalc, layoutRow, 1, 1, 2) 
        self.leResultSpcZcalc.setEnabled(False)
        #Add the result zref
        layoutRow += 1
        self.lblResultZref = QtGui.QLabel('        - zref', wdg)
        layout.addWidget(self.lblResultZref, layoutRow, 0, 1, 1) 
        self.leResultSpcZref = QtGui.QLineEdit(wdg)
        self.leResultSpcZref.setFixedWidth(100) 
        layout.addWidget(self.leResultSpcZref, layoutRow, 1, 1, 2) 
        self.leResultSpcZref.setEnabled(False)  

        
        #Add the show parameters section separator
                
        self.lblShowParametersSection = QtGui.QLabel('----------', wdg)
        layout.addWidget(self.lblShowParametersSection, 15, 0, 1, 1) 
        
        #Add the show button
        self.btn = QtGui.QPushButton('Show', wdg)
        self.btn.setFixedWidth(500)
        self.btn.setFixedHeight(50)
        self.btn.clicked.connect(self.bt_showAViewWidget)
        self.btn.setToolTip('Display the Chi2/spectrum/fitted template/linemodel results successively...')
        layout.addWidget(self.btn, 16, 1, 1, 1)

        #Add the extremum choice ctrls
        self.lblExtremumChoice = QtGui.QLabel(' Extremum: ', wdg)
        layout.addWidget(self.lblExtremumChoice, 18, 0, 1, 1)
        
        self.leExtremumChoice = QtGui.QLineEdit(wdg)
        self.leExtremumChoice.setToolTip('Enter the extremum num to be selected for display...')
        self.leExtremumChoice.setFixedWidth(100) 
        self.leExtremumChoice.setEnabled(False)
        layout.addWidget(self.leExtremumChoice, 18, 1, 1, 10)
        
        self.ckExtremumChoiceOverride = QtGui.QCheckBox('Extremum Override', wdg)
        self.ckExtremumChoiceOverride.stateChanged.connect(self.ck_extremumOverride)
        layout.addWidget(self.ckExtremumChoiceOverride, 18, 2, 1, 10)
        #self.ckExtremumChoiceOverride.toggle()
        
        #self.setCentralWidget(wdg)
        self.setLayout(layout)        
        self.setWindowTitle('Aview')
        

        #Set path from settings
        self.settings = QtCore.QSettings("amazed", "aviewgui")
        _resDir = self.settings.value("resDir").toString()
        self.leResDir.setText(_resDir)      
        if _resDir=="":
            self.enableCtrls(False)
            
        #Set spc filter
        self.leSpcFilter.setText("Not implement yet...")
        self.leSpcFilter.setEnabled(False)
            
        #Set zdiff Threshold
        self.leDiffThres.setText("")
            

        #auto load from inputs args
        if not init_resdir=="" and not init_refpath=="":
            self.leResDir.setText(init_resdir) 
            self.bt_loadresults(init_refpath)
            
        wdg.setStyleSheet("*[coloredcell=\"true\"] {background-color:rgb(215,215,215);}")
        self.show()
 
    def bt_loadresults(self, init_refPath=""):
        self.setCurrentDir()
    
        _resDir = str(self.leResDir.text())
        print("using _resDir: {}".format(_resDir))
        
        statsPath = os.path.join(_resDir, "stats")
        diffPath = os.path.join(statsPath, "diff.txt")
        if not os.path.exists(diffPath):
            print("diff file not found... computing the diff file is necessary: please select the reference redshift file list in order continue !")
            calcFile = os.path.join(_resDir, "redshift.csv") 
            
            if not init_refPath=="":
                _refzfilepath = init_refPath
            else:
                _refzfilepathDefault = self.settings.value("refzfilepath").toString()
                _refzfilepath = str(QtGui.QFileDialog.getOpenFileName(self, "Select Reference Redshift list file", _refzfilepathDefault))
            if os.path.exists(_refzfilepath) :
                if os.path.isdir(statsPath)==False:
                    os.mkdir( statsPath, 0755 );
                self.settings.setValue("refzfilepath", _refzfilepath)
                processAmazedOutputStats.setPFSRefFileType()
                processAmazedOutputStats.ProcessDiff( _refzfilepath, calcFile, diffPath , reftype='pfs')
            else:
                print("Selected zref file does not exist...")
        
        _spcName = str(self.leSpcFilter.text()) #todo: filter the results by name tag
        _diffthres = str(self.leDiffThres.text())
        if _diffthres=="":
            _diffthres = -1
        
        self.resList = resultstat.ResultList(_resDir, diffthreshold=float(_diffthres), opt='brief') 
        self.leResultsN.setText(str(self.resList.n))
        #init result index to 1 (first index)
        self.leResultIndex.setText(str(1))
        self.refreshResultDetails()

    def refreshResultDetails(self):
        current_index = int(self.leResultIndex.text() )-1
        self.leResultSpcName.setText(self.resList.list[current_index].name)
        self.leResultSpcZdiff.setText(str(self.resList.list[current_index].zdiff))
        self.leResultSpcZcalc.setText("{:.5}".format(self.resList.list[current_index].zcalc))
        self.leResultSpcZref.setText("{:.5}".format(self.resList.list[current_index].zref))
        
    
    def bt_nextResultIndex(self):
        current_index = int(self.leResultIndex.text() )
        if current_index>self.resList.n-1:
            return
        self.leResultIndex.setText(str(current_index+1))  
        self.refreshResultDetails()  

    def bt_previousResultIndex(self):
        current_index = int(self.leResultIndex.text() )
        if current_index<2:
            return
        self.leResultIndex.setText(str(current_index-1))
        self.refreshResultDetails()
        
        
    #deprecated...
    def bt_showDialog(self):
        #self.setCurrentDir()
        _resDir = str(self.leResDir.text())
        print("using _resDir: {}".format(_resDir))
        
        statsPath = os.path.join(_resDir, "stats")
        diffPath = os.path.join(statsPath, "diff.txt")
        if not os.path.exists(diffPath):
            print("diff file not found... computing the diff file is necessary: please select the reference redshift file list in order continue !")
            calcFile = os.path.join(_resDir, "redshift.csv") 
            
            _refzfilepathDefault = self.settings.value("refzfilepath").toString()
            _refzfilepath = str(QtGui.QFileDialog.getOpenFileName(self, "Select Reference Redshift list file", _refzfilepathDefault))
            if os.path.exists(_refzfilepath) :
                if os.path.isdir(statsPath)==False:
                    os.mkdir( statsPath, 0755 );
                self.settings.setValue("refzfilepath", _refzfilepath)
                processAmazedOutputStats.setPFSRefFileType()
                processAmazedOutputStats.ProcessDiff( _refzfilepath, calcFile, diffPath )
            else:
                print("Selected zref file does not exist...")
        
        _spcName = str(self.leSpcFilter.text())
        _tplpath = ""
        _redshift = ""
        _iextremaredshift = 0
        _diffthres = str(self.leDiffThres.text())
        if _diffthres=="":
            _diffthres = -1
        _failureindex = "0"
        aview.plotRes(_resDir, _spcName, _tplpath, _redshift, _iextremaredshift, _diffthres, _failureindex)

    #not used, this just calls the aview command line interface
    def bt_showResult(self):
        _resDir = str(self.leResDir.text())
        
        _spcName = str(self.leResultSpcName.text())
        _tplpath = ""
        
        #current_index = int(self.leResultIndex.text() )-1
        _redshift = ""#self.resList.list[current_index].zcalc

                
        if self.ckExtremumChoiceOverride.isChecked():
            iextrFromCtrl = str(self.leExtremumChoice.text())
            if iextrFromCtrl=="":
                _iextremaredshift = 0
            else:
                _iextremaredshift = float(iextrFromCtrl)
        else:
            _iextremaredshift = 0
            
        _diffthres = str(self.leDiffThres.text())
        if _diffthres=="":
            _diffthres = -1
        _failureindex = "0"
        aview.plotRes(_resDir, _spcName, _tplpath, _redshift, _iextremaredshift, _diffthres, _failureindex)
        
        
    def bt_showAViewWidget(self):
        
        _resDir = str(self.leResDir.text())
        
        _spcName = str(self.leResultSpcName.text())
        _spcIdx = -1
        for idx in range(self.resList.n):
            if _spcName == self.resList.list[idx].name:
                print("INFO: found index for this spectrum and diffthreshold : i={}\n".format(idx))
                _spcIdx = idx
                break
            
        if self.ckExtremumChoiceOverride.isChecked():
            iextrFromCtrl = str(self.leExtremumChoice.text())
            if iextrFromCtrl=="":
                _iextremaredshift = 0
            else:
                _iextremaredshift = float(iextrFromCtrl)
        else:
            _iextremaredshift = 0
            
        _resParser = resparser.ResParser(_resDir)

        self.AViewWidget = aviewwidget.AViewWidget(parent=None, resParser=_resParser, resList=self.resList, resIdx=_spcIdx, iextremaredshift=_iextremaredshift)
        self.AViewWidget.show()
        
    
    def bt_setResultDir(self):
        _resDirDefault = os.path.abspath(str(self.leResDir.text()))
        _resDirDefault = _resDirDefault[:_resDirDefault.index(os.sep)] if os.sep in _resDirDefault else _resDirDefault
        
        _resDir = str(QtGui.QFileDialog.getExistingDirectory(self, "Select Directory",_resDirDefault))
        if os.path.exists(_resDir):
            #check the diff file is present, if not, do something...
            print("_resDir = {}".format(_resDir))
            print("previous from settings = {}".format(self.settings.value("resDir").toString()))
            
            if not _resDir == self.settings.value("resDir").toString():
                _resDirPrevious = self.settings.value("resDir").toString()
                self.settings.setValue("resDirPrevious", _resDirPrevious);
            self.leResDir.setText(_resDir)
            self.settings.setValue("resDir", _resDir);
            self.enableCtrls(True)
        
    def bt_setResultDirPrevious(self):
        _resDirPrevious = self.settings.value("resDirPrevious").toString()
        self.leResDir.setText(_resDirPrevious)
        self.settings.setValue("resDir", _resDirPrevious);
        self.enableCtrls(True)
        
    def ck_extremumOverride(self, state):
        if state == QtCore.Qt.Checked:
            self.leExtremumChoice.setEnabled(True)
            iextrFromCtrl = str(self.leExtremumChoice.text())
            if iextrFromCtrl=="":
                self.leExtremumChoice.setText("0")
        else:
            self.leExtremumChoice.setEnabled(False)
            
    def enableCtrls(self, val):
        self.leDiffThres.setEnabled(val)
        #self.leSpcFilter.setEnabled(val)
        self.btn.setEnabled(val)
    
    def setCurrentDir(self):
        _savedCurDir = os.path.realpath(os.path.abspath(os.path.curdir))
        _resDir = str(self.leResDir.text())
        _resParser = resparser.ResParser(_resDir)

        #fixed
        #_amazedDir = "/home/aschmitt/data/pfs/pfs2_simu20151118_jenny/amazed"    
        #os.chdir(_amazedDir)
    
        #try to find the amazed working directory by identifying the linecatalog relative path
        _abscurpath = os.path.realpath(os.path.abspath(_resDir))
        _relCatalogDir = os.path.normpath( _resParser.getConfigVal("linecatalog"))
        if len(glob.glob(_relCatalogDir))>0:
            return
        #print("_relCatalogDir = {}".format(_relCatalogDir))
        continueDown = True;
        while continueDown:
            os.chdir(_abscurpath)
            #print("\ncurdir = {}".format(_abscurpath))
            subfolder = "../"      
            
            _possible_root = glob.glob(_relCatalogDir) 
            _possible_level1 = glob.glob("*/" + _relCatalogDir)
            _possible_level2 = glob.glob("*/" + "*/" + _relCatalogDir)            
            #print("_possible_root = {}".format(_possible_root))     
            #print("_possible_level1 = {}".format(_possible_level1))
            
            if _abscurpath==os.path.sep:
                continueDown = False
                os.chdir(_savedCurDir)
            elif len(_possible_root)==1:
                continueDown = False
                print("SETTING CURRENT DIRECTORY: _amazedDir = {}".format(_abscurpath))
            elif len(_possible_level1)==1:
                subdir = _possible_level1[0].split(os.path.sep)[0]
                _amazedDir = os.path.join(_abscurpath, subdir)
                os.chdir(_amazedDir)
                continueDown = False
                print("SETTING CURRENT DIRECTORY: _amazedDir = {}".format(_amazedDir))
            elif len(_possible_level2)==1:
                subdir0 = _possible_level2[0].split(os.path.sep)[0]
                subdir1 = _possible_level2[0].split(os.path.sep)[1]
                _amazedDir = os.path.join(_abscurpath, subdir0, subdir1)
                os.chdir(_amazedDir)
                continueDown = False
                print("SETTING CURRENT DIRECTORY: _amazedDir = {}".format(_amazedDir))
            else:
                _abscurpath = os.path.realpath(os.path.abspath(os.path.join(_abscurpath,subfolder)))  
 
def main():
    app = QtGui.QApplication(sys.argv)
    ins = ObjectAViewGui()
    ex = AViewGui()
    sys.exit(app.exec_())
 
if __name__ == '__main__':
    main()