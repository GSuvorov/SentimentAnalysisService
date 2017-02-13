/** \mainpage
<b>1. ����� ��������</b>\n
������ NdxDataStorage ������������ ��� �������� ��������� �������� ������ ���� ����������������� �������. \n
������ ���������� �� ����� ���������������� C++ � �������������� ���������������� \n
������� �� ����� ���������� Microsoft Visual Studio .Net, ���������� STL. \n

<b>2. �������������� ����������</b>\n
������ ������ ������ ��������� ������:
\li	�������� ��������� �������� ������ (������� �������� ������ ���� ����������������� �������).
�������������� ����������� ���.

<b>3. �������� ���������� ���������</b>\n
������ ������ ������������� ��������� ����������:
\li	INdxDataUnit  ��������� ������ � ������� ������ ������ ��������� ����
\li -> Create ������� ���� ������ 
\li -> Open ��������� ���� ������ � ������� ������
\li -> Close ��������� ���� ������ 
\li -> Flush ���������� ���� ������
\li -> Clear �������  ���� ������ 
\li -> GetDataUnitSize ���������� ������ ����� ������ 
\li -> IsOpen ��������� ������ �� ���� ������ 
\li -> GetDataUnitName ���������� ��� ����� ������ 
\li -> GetDataUnitExt ���������� ���������� ����� ������ 
\n
\n

\li	INdxFile ��������� ������ � ������� ��������� ������
\li -> Read ������ �� ����� �� ������� �������
\li -> Write ����� � ���� �� ������� �������
\li -> Append ��������� � �����
\li -> SeekPos �������� ������� ������� � �����
\li -> TellPos ���������� ������� ������� � �����
\li -> Reserve ����������� ����� � �����
\li -> Rename ���������������, ���������� ����
\li -> IsBad ��������� ����� ����� �� ����������
\li -> GetFileHandle ���������� ����� �����
\n
\n

\li	INdxCashedFile ��������� ������ � ��������� ������ �������������� ����������� ������-������
\li -> InitCash �������������� ��������� ����
\li -> Cashed �������� ����
\li -> FlushFilledBuffers ���������� �� ���� ����������� ������ ���� �����
\n
\n

\li	INdxLevelStorage ��������� ������ � ���������� ������ ��������� ������
\li -> GetLevelInfo ���������� ���������� �� ������
\li -> SeekPos ����������� �� ��������y ��������� ������
\li -> TellPos ���������� ������� � ��������� ��������� ������
\li -> StartRead �������� ������ �� ������ �������
\li -> Read ������ � �����
\li -> StopRead ����������� ������, ���������� ������ ������������
\li -> StartCompleteDataAppend �������� ���������� ������������ ������
\li -> StartUnCompleteDataAppend �������� ���������� ���������� ������
\li -> Append ���������� ������ ������
\li -> StopAppend ����������� ����������, ���������� ������ ������������
\li -> GetAppendedUnCompleteDataSize ���������� ������ ���������� ���������� ������
\li -> SeekPosInAppendedUnCompleteData ������������� � ���������� ���������� ������
\li -> WriteInAppendedUnCompleteData ����� ����� ������ � ���������� ���������� ������
\li -> Delete �������� �� ��������� ������
\li -> UpdateData ��������� ������ � ��������� ��������� ������
\n
\n

\li	INdxTableStorage ��������� ������ � ��������� ���������� ������
\li -> AddField ��������� ���� � �������
\li -> GetFieldsCollection ���������� ��������� ����� �������
\li -> InitFind �������������� ������� ������ � ����
\li -> Find ���� � ������� ��������� ��� �� ������������ ��������� ���� ����� �� ���������� ��������
\li -> FindOne ���� � ������� ���� ��� �� ����������� �������
\li -> SpanFindInit �������������� ������� ������������� ������ � ����
\li -> SpanFind ���� � ������� ������ ��������������� SpanFindInit
\li -> GetFoundRecordIndex ���������� ������ ������ ������� �������� ����� ������� ������� ������
\li -> DeleteRecord ������� ������ ������� �� �������
\li -> InitScan �������������� ������� ������������ �������
\li -> Scan ���������� ��������� ������ �������
\li -> ScanOne ���������� ������ ������� �� �������
\li -> InitAdd �������������� �������� ���������� ������ � �������.
\li -> Add ��������� ������ � �������
\li -> InitModify �������������� �������� ��������� ���� ����� �������.
\li -> FindOneAndChange ������� � �������� ������ � �������
\n
\n

\li	INdxVectorStorage ��������� ������ � ��������� ���������� ������
\li -> MoveToAndGetDataSize ��������� �� ������ ������ �� �������, ���������� �� ������
\li -> ReadWholeData ������ ��� ������ ����������� �� �������
\li -> ReadDataChunk ������ ����� ������ ����������� �� �������
\li -> SeekInData ������� ������ ������ ����������� �� �������, �������� ��������� �� ������
\li -> AppendWholeData ��������� ������ � ���������, ���������� ������, �� �������� ����� ��������� ������
\li -> AppendDataChunk ��������� ����� ������, ��������� �������� ��������� ����������
\li -> AppendDataChunkComplete ��������� �������� ��������� ����������, ���������� ������, �� �������� ����� ��������� ������
\li -> Delete ������� ������ �� �������
\li -> IsDeleted ��������� ������� �� ������ �� �������
\li -> GetIndexesNumber ���������� ���������� ��������
\n
\n

\li	INdxStorage ��������� ������ � ���������� ��������� ������
\li -> GetEntryPointsStorage ���������� ��������� ����� ����� � ������
\li -> GetTextImagesStorage ���������� ��������� ������� ����������
\li -> AddLevelStorage ��������� ��������� ������ � �������
\li -> GetLevelStorage ���������� ��������� ������ �� ������
\li -> GetLevelStoragesNumber ���������� ����� ���������� ������
\n
\n

\li	INdxStorageWithStatistic ��������� ������ � ���������� ��������� ������
\li -> GetIndexFrequencyStorage ���������� ��������� ������ ��������
\li -> GetDocumentNormStorage ���������� ��������� ���� ���������� �������
\n
\n

\li	INdxDataStorageFactory
\li -> CreateNdxFile ������� ������� ����
\li -> CreateNdxCashedFile  ������� ���� �������������� ����������� ������-������
\li -> CreateNdxTableStorage ������� ��������� ��������� ������
\li -> CreateNdxVectorStorage ������� ��������� ��������� ������
\li -> CreateNdxStorage ������� ��������� ��������� ������
\li -> CreateNdxStorageWithStatistic ������� ��������� ��������� ������ � ���������� ����������
\n
\n

<b>4. ������������ ����������� ��������</b>\n
������������ ����������� ����������� ����, �� ������� ����� ���� ����������� �� Windows.

<b>5. ����� � ��������</b>\n
\li �������� ������ ���������� ����������� ��� ������ �������. 
\li ����� �����:
\li CreateInstance - ����� ����� ��� ������������ �������� �����������.
\li DllMain - ����������� ����� �����.

<b>6. ������� ������</b>\n
\li ������������� ����������(������).
\li	������������� ������.
\li	������������� ����������.
\li ��� ���������
\li ������� ����

<b>7. �������� ������</b>\n
\li ��������� �� ��������� ���������
\li �������� ���������

*/