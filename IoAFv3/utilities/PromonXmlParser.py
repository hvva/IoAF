import xml.etree.ElementTree as ET
import re
import getopt
import sys
import os
import sqlite3


class CSigDbInto:
	def __init__(self,a_sToolName,a_sFilePath,a_sRegPath):
		self.m_sToolName = a_sToolName
		self.m_sFilePath = a_sFilePath
		self.m_sRegPath = a_sRegPath


	def InsertData(self):	
		
		self.m_db = sqlite3.connect('sig.db')
		self.m_cur = self.m_db.cursor()

		self.m_cur.execute('create table if not exists registry (action NUMERIC, tool NUMERIC, key TEXT)')
		self.m_cur.execute('create table if not exists file (tool NUMERIC, action NUMERIC, full_path TEXT)')
		self.m_cur.execute('create table if not exists tool (type text, detail text, name text, tool_num INTEGER PRIMARY KEY)')

		self.m_cur.execute("select max(tool_num) from tool")
		row = self.m_cur.fetchone()

		total_tool = 0 if row[0] == None else row[0]+1
		self.m_cur.execute( 'insert into tool values ( "", "", (?), (?))', (self.m_sToolName, total_tool) )	

		self.m_cur.execute('insert into registry values (2, (?), (?))', ( total_tool, self.m_sRegPath ) )
		self.m_cur.execute('insert into file values ((?), 2, (?))',  ( total_tool, self.m_sFilePath ) )			
		
		self.m_cur.close()
		self.m_db.commit()
		self.m_db.close()		


		

class CProMonXmlParser:
	def __init__(self,a_sFileName=None):
		self.m_Root = None
		self.m_ProcessList = None
		self.m_EventList = None		
		self.m_RegSet = set()	
		self.m_FileSet = set()	
		self.m_sFilePath = None
		self.m_sRegPath = None

		if a_sFileName != None:
			tree = ET.parse(a_sFileName)			
			self.m_Root = tree.getroot()
			self.m_ProcessList = self.m_Root[0]
			self.m_EventList = self.m_Root[1]
						

	def GetList(self):
		for event in self.m_EventList.findall('event'):
			sOperation = event.find('Operation').text
			sPath = event.find('Path').text
			sResult = event.find('Result').text
			sDetail = event.find('Detail').text

			if (sOperation == "CreateFile") and (sResult == "SUCCESS"):
				sPath = os.path.dirname(sPath)
				self.m_FileSet.add(sPath)

			m = re.match('Reg',sOperation)

			if (m != None) and (sResult == "SUCCESS"):
				sPath = os.path.dirname(sPath)
				self.m_RegSet.add(sPath)				


	def Show(self):
		print "[CreateFile List]"
		index = 0

		for k in self.m_FileSet:
			print index,k
			index = index + 1;

		print "[RegAccess List]"
		index  = 0
		for k in self.m_RegSet:
			print index,k
			index = index + 1;

	def SelectGetIndex(self):
		iFileIndex = raw_input("Select CreateFile Index : ")
		iRegIndex = raw_input("Select Reg Index : ")

		
		vListFile = list(self.m_FileSet)	
		vListReg = list(self.m_RegSet)


		iFileIndex = int(iFileIndex)
		iRegIndex = int(iRegIndex)

		sFilePath = vListFile[iFileIndex]
		sRegPath = vListReg[iRegIndex]

		if sFilePath.find('AppData') != -1:
			sFilePath = sFilePath.split('AppData')[1]

		
		print sFilePath
		print sRegPath	

		self.m_sFilePath = sFilePath
		self.m_sRegPath = sRegPath	
		

	def Start(self):	
		self.GetList()
		self.Show()
		self.SelectGetIndex()


			

if __name__  == '__main__':

	try:
		sFileName =  sys.argv[1]
		c = CProMonXmlParser(sFileName)
		c.Start()

		sToolName = raw_input('Input Tools Name : ')
		sFilePath = c.m_sFilePath
		sRegPath = c.m_sRegPath
		
		SigDb = CSigDbInto(sToolName,sFilePath,sRegPath)
		SigDb.InsertData()
	
	except:
		print "Error"	
		print "Usage: python "+ os.path.basename(__file__) +" file.xml"
		print "\t*File.xml should use the same xml structure as procmon."
