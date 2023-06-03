/* 
   isDcc
   (c) 1998 Andrew de Quincey
   adq@tardis.ed.ac.uk
   See README.TXT for copying/distribution/modification details.
*/

#ifndef DECODETABLE_H
#define DECODETABLE_H

#include "common.h"
#include "decodeOps.h"

static char* rawDecodeTable[][4] = {
                             // these are all the system defined functions
			     {(char*) 0x013a, "AddFolderIcon", (char*) 8, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ee, "AddProfString", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0018, "AddProgItem", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x00c9, "AddProgItemEx", (char*) 8, (char*) (*parseSystemFunction)},
			     {(char*) 0x00b9, "AddressNumber", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ba, "AddressString", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00bb, "AddressStruct", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0081, "AppCommand", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x002e, "AppendToPath", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0137, "AskDestPath", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0020, "AskOptions", NULL, (char*) (*askOptions)},
			     {(char*) 0x000d, "AskPath", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x001e, "AskText", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0028, "AskYesNo", (char*) 2, (char*) (*parseSystemFunction)},

			     {(char*) 0x004b, "BatchAdd", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x004c, "BatchDelete", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d8, "BatchDeleteEx", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0048, "BatchFileLoad", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x004d, "BatchFileSave", (char*) 1, (char*) (*parseSystemFunction)}, // = BatchSave
			     {(char*) 0x0049, "BatchFind", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0047, "BatchGetFileName", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x004a, "BatchMove", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d7, "BatchMoveEx", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0062, "BatchSetFileName", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x007f, "BitAnd", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0080, "BitOr", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x007e, "BitNot", (char*) 1, (char*) (*parseSystemFunction)},

			     {(char*) 0x013b, "CallDLLFx", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0082, "ChangeDirectory", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x001f, "CloseFile", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x009a, "CmdGetHwndDlg", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x009c, "CmdGetParam1", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x009d, "CmdGetParam2", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x009b, "CmdGetMsg", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x014f, "CommitSharedFiles", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x014a, "ComponentAddItem", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0172, "ComponentCompareSizeRequired", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0149, "ComponentDialog", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0173, "ComponentError", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0174, "ComponentFileEnum", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0175, "ComponentFileInfo", (char*) 6, (char*) (*parseSystemFunction)},
			     {(char*) 0x0176, "ComponentFilter", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0197, "ComponentFilterLanguage", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0196, "ComponentFilterOS", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0177, "ComponentGetData", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0161, "ComponentGetItemInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0157, "ComponentGetItemSize", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0178, "ComponentInitialize", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x014b, "ComponentIsItemSelected", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0156, "ComponentListItems", (char*) 2, (char*) (*doComponentListItems)},
			     {(char*) 0x0163, "ComponentLink", (char*) 7, (char*) (*parseSystemFunction)},
			     {(char*) 0x0179, "ComponentMoveData", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x017a, "ComponentMoveDataEz", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x014c, "ComponentSelectItem", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x017b, "ComponentSetData", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0193, "ComponentSetCurrent", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0160, "ComponentSetItemInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0192, "ComponentSetTarget", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x018a, "ComponentSetupTypeEnum", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0194, "ComponentSetupTypeGetData", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0189, "ComponentSetupTypeSet", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0187, "ComponentTextSubEnum", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0188, "ComponentTextSubEval", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0186, "ComponentTextSubQuery", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x017c, "ComponentTextSubSet", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0162, "ComponentTotalSize", (char*) 2, (char*) (*doComponentTotalSize)},
			     {(char*) 0x017d, "ComponentValidate", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x018b, "ComponentViewCreate", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0190, "ComponentViewCreateWindow", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x018f, "ComponentViewDestroy", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x018d, "ComponentViewQueryInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x018e, "ComponentViewRefresh", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0191, "ComponentViewSelectAll", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x018c, "ComponentViewSetInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x00c0, "CompressAdd", (char*) 3, (char*) (*parseSystemFunction)},   // IS file version 1 only
			     {(char*) 0x00c1, "CompressGet", (char*) 3, (char*) (*parseSystemFunction)},   // IS file version 1 only
			     {(char*) 0x00c2, "CompressDel", (char*) 3, (char*) (*parseSystemFunction)},   // IS file version 1 only
			     {(char*) 0x00c3, "CompressEnum", (char*) 4, (char*) (*parseSystemFunction)},  // IS file version 1 only
			     {(char*) 0x00c4, "CompressInfo", (char*) 5, (char*) (*parseSystemFunction)},  // IS file version 1 only
			     {(char*) 0x0056, "ConfigAdd", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0057, "ConfigDelete", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0053, "ConfigFileLoad", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x005a, "ConfigFileSave", (char*) 1, (char*) (*parseSystemFunction)}, // = ConfigSave
			     {(char*) 0x0054, "ConfigFind", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0052, "ConfigGetFileName", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0058, "ConfigGetInt", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0055, "ConfigMove", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0063, "ConfigSetFileName", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0059, "ConfigSetInt", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0079, "CopyBytes", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x001c, "CopyFile", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00e5, "CopyFileEx", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0010, "CreateDir", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0011, "CreateFile", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x013c, "CreateProgramFolder", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0017, "CreateProgGroup", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00c7, "CreateProgGroupEx", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x019a, "CreateRegistrySet", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0199, "CreateShellObjects", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0096, "CtrlClear", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0097, "CtrlDir", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0094, "CtrlGetCurSel", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f0, "CtrlGetMLEText", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f2, "CtrlGetMultCurSel", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x008f, "CtrlGetState", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x009e, "CtrlGetSubCommand", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0092, "CtrlGetText", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0098, "CtrlPGroups", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0093, "CtrlSelectText", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0095, "CtrlSetCurSel", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0106, "CtrlSetFont", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0099, "CtrlSetList", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f1, "CtrlSetMLEText", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f9, "CtrlSetMultCurSel", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0090, "CtrlSetState", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0091, "CtrlSetText", (char*) 3, (char*) (*parseSystemFunction)},

			     {(char*) 0x008b, "DefineDialog", (char*) 8, (char*) (*parseSystemFunction)},
			     {(char*) 0x0145, "DeinstallStart", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0195, "DeinstallSetReference", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x000a, "Delay", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x003f, "DeleteDir", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x001d, "DeleteFile", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x013e, "DeleteFolderIcon", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0066, "DeleteGroup", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00cb, "DeleteProgItem", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x013d, "DeleteProgramFolder", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0198, "DialogSetFont", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x015f, "DialogSetInfo", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0002, "Disable", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0077, "Divide", (char*) 2, (char*) (*parseSystemFunction)},  // "Div"?
			     {(char*) 0x0154, "Do", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00e4, "DoInstall", (char*) 3, (char*) (*parseSystemFunction)},

			     {(char*) 0x012a, "Effects", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0001, "Enable", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x008d, "EndDialog", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x001a, "EnterDisk", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0034, "ExecuteDLL", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x000c, "ExistsDir", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x000b, "ExistsDisk", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d1, "ExitProgMan", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0038, "ExportNum", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0039, "ExportStr", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0045, "EzBatchAddPath", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0046, "EzBatchAddString", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0083, "EzBatchReplace", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x004e, "EzConfigAddDriver", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x004f, "EzConfigAddString", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0050, "EzConfigGetValue", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0051, "EzConfigSetValue", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0113, "EzDefineDialog", (char*) 4, (char*) (*parseSystemFunction)},

			     {(char*) 0x00dd, "FileCompare", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00e0, "FileDeleteLine", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00de, "FileGrep", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x00df, "FileInsertLine", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d2, "FileSetBeginDefine", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d3, "FileSetEndDefine", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d6, "FileSetEndPerform", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d5, "FileSetPerform", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x013f, "FileSetPerformEz", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d4, "FileSetReset", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00fc, "FileSetRoot", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0101, "FindAllDirs", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0040, "FindAllFiles", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0029, "FindFile", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0073, "FindWindow", (char*) 2, (char*) (*parseSystemFunction)},

			     {(char*) 0x007a, "GetByte", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x000f, "GetDir", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x000e, "GetDisk", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0005, "GetDiskSpace", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x019b, "GetDiskSpaceEx", (char*) 2, (char*) (*parseSystemFunction)}, // installshield 5.5+ only 
			     {(char*) 0x0041, "GetEnvVar", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0035, "GetExtents", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x007c, "GetFileInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0165, "GetFolderNameList", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0105, "GetFont", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0072, "GetGroupNames", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00c8, "GetGroupNameList", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ca, "GetItemNameList", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0037, "GetLine", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0006, "GetMemFree", (char*) 0, (char*) (*parseSystemFunction)},
			     {(char*) 0x0007, "GetMode", (char*) 0, (char*) (*parseSystemFunction)},
			     {(char*) 0x0027, "GetProfInt", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0026, "GetProfString", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x016e, "GetScriptFunctionAddress", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0061, "GetSystemInfo", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00e1, "GetValidDrivesList", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00c6, "GetWindowHandle", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0084, "Handler", (char*) 2, (char*) (*doHandler)},

			     {(char*) 0x0146, "InstallationInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x017e, "IndirectNumber", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00e2, "Is", (char*) 2, (char*) (*parseSystemFunction)},
			     //			     {(char*) 0x0???, "ISCompareServicePack", (char*) 1, (char*) (*parseSystemFunction)},

			     
			     {(char*) 0x0114, "Language", (char*) 1, (char*) (*parseSystemFunction)}, // = LanguageSet
			     {(char*) 0x0118, "LanguageInfo", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0033, "LaunchApp", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00e3, "LaunchAppAndWait", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a3, "ListAddItem", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a1, "ListAddString", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f8, "ListCount", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x009f, "ListCreate", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f5, "ListCurrentItem", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f4, "ListCurrentString", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a8, "ListDeleteString", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f6, "ListDeleteItem", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a0, "ListDestroy", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00f7, "ListFindItem", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a2, "ListFindString", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0117, "ListGetFileResource", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a6, "ListGetFirstItem", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a4, "ListGetFirstString", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a7, "ListGetNextItem", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a5, "ListGetNextString", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0116, "ListReadFromFile", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00aa, "ListSetCurrentItem", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00a9, "ListSetCurrentString", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0155, "ListSetIndex", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0115, "ListWriteToFile", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x015c, "LongPathFromShortPath", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x015a, "LongPathToQuote", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x015b, "LongPathToShortPath", (char*) 1, (char*) (*parseSystemFunction)},

			     {(char*) 0x003a, "MessageBeep", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x002a, "MessageBox", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0171, "MMediaIsPlaying", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0076, "Mul", (char*) 2, (char*) (*parseSystemFunction)},

			     {(char*) 0x006e, "NumToStr", (char*) 2, (char*) (*parseSystemFunction)},

			     {(char*) 0x0024, "OpenFile", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0065, "OpenFileMode", (char*) 1, (char*) (*parseSystemFunction)},

			     {(char*) 0x00d9, "ParsePath", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x005d, "PathAdd", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0060, "PathDelete", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x005f, "PathFind", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x005c, "PathGet", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x005e, "PathMove", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x005b, "PathSet", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0036, "PlaceBitmap", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0068, "PlaceWindow", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0170, "PlayMMedia", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0111, "ProgDefGroupType", (char*) 1, (char*) (*parseSystemFunction)},

			     {(char*) 0x00cd, "QueryProgGroup", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00cc, "QueryProgItem", (char*) 8, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ce, "QueryShellMgr", (char*) 1, (char*) (*parseSystemFunction)},

			     {(char*) 0x006b, "ReadBytes", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x014d, "RebootDialog", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x016a, "RegDBConnectRegistry", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0085, "RegDBCreateKey", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0150, "RegDBCreateKeyEx", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0086, "RegDBCreateKeyValue", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0087, "RegDBDeleteKey", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0166, "RegDBDeleteValue", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x016b, "RegDBDisConnectRegistry", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0142, "RegDBGetAppInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0148, "RegDBGetItem", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0089, "RegDBGetKeyValue", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0152, "RegDBGetKeyValueEx", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0088, "RegDBKeyExist", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x016c, "RegDBMergeRegFile", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0164, "RegDBQueryKey", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0141, "RegDBSetAppInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0110, "RegDBSetDefaultRoot", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0147, "RegDBSetItem", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x008a, "RegDBSetKeyValue", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0151, "RegDBSetKeyValueEx", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x008e, "ReleaseDialog", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00cf, "ReloadProgGroup", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0078, "RenameFile", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0143, "ReplaceFolderIcon", (char*) 10, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ef, "ReplaceProfString", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x00d0, "ReplaceProgItem", (char*) 9, (char*) (*parseSystemFunction)},

			     {(char*) 0x006a, "SeekBytes", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0158, "SelectDir", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0139, "SelectFolder", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0074, "SendMessage", (char*) 4, (char*) (*parseSystemFunction)}, 
			     {(char*) 0x0003, "SetColor", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x007b, "SetByte", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x003b, "SetDialogTitle", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x016f, "SetDisplayEffect", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x003d, "SetErrorMsg", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x003e, "SetErrorTitle", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x007d, "SetFileInfo", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x003c, "SetFont", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0009, "SetInfoWindow", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0008, "SetStatusWindow", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0004, "SetTitle", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0138, "SetupType", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0067, "ShowGroup", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x014e, "ShowProgramFolder", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x015d, "SilentReadData", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x015e, "SilentWriteData", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0069, "SizeWindow", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00e6, "Split", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0103, "Sprintf", NULL, (char*) (*sPrintf)},
			     {(char*) 0x0104, "SprintfBox", NULL, (char*) (*sPrintfBox)},
			     {(char*) 0x0064, "StatusUpdate", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00eb, "StatusGetSize", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ea, "StatusSetLine", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0023, "StrCompare", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0014, "StrConcat", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0031, "StrFind", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00dc, "StrGetTokens", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x002f, "StrLength", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0112, "StrLoadString", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x0153, "StrRemoveLastSlash", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0030, "StrSub", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x00da, "StrToLower", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x006d, "StrToNum", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00db, "StrToUpper", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00bc, "StructGet", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x00be, "StructPut", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x017f, "StructGetAddressEx", (char*) 5, (char*) (*parseStructFunction)},
			     {(char*) 0x0180, "StructGetPAddressEx", (char*) 5, (char*) (*parseStructFunction)},
			     {(char*) 0x00bd, "StructGetP", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0181, "StructGetEx", (char*) 5, (char*) (*parseStructFunction)},
			     {(char*) 0x0182, "StructGetPEx", (char*) 5, (char*) (*parseStructFunction)},
			     {(char*) 0x0183, "StructPutEx", (char*) 5, (char*) (*parseStructFunction)},
			     {(char*) 0x00bf, "StructPutP", (char*) 5, (char*) (*parseSystemFunction)},
			     {(char*) 0x0184, "StructPutPEx", (char*) 5, (char*) (*parseStructFunction)},
			     {(char*) 0x00f3, "StructSizeOf", (char*) 1, (char*) (*parseStructFunction)},
			     {(char*) 0x0185, "StructSizeOfEx", (char*) 1, (char*) (*parseStructFunction)},
			     {(char*) 0x0042, "SuppressErrorMsg", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x006f, "System", (char*) 1, (char*) (*parseSystemFunction)},

			     {(char*) 0x00b3, "UnUseDLL", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00b2, "UseDLL", (char*) 1, (char*) (*parseSystemFunction)},

			     {(char*) 0x0100, "VarRestore", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x012b, "VarLoadFromFile", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ff, "VarSave", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x012c, "VarSaveToFile", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0102, "VerCompare", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ac, "VerFindFileVersion", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ab, "VerGetFileVersion", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x012d, "VarSaveListToFile", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ad, "VerSearchAndUpdateFile", (char*) 3, (char*) (*parseSystemFunction)},
			     {(char*) 0x00ae, "VerUpdateFile", (char*) 3, (char*) (*parseSystemFunction)},

			     {(char*) 0x008c, "WaitOnDialog", (char*) 1, (char*) (*parseSystemFunction)},
			     {(char*) 0x0144, "Welcome", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x006c, "WriteBytes", (char*) 4, (char*) (*parseSystemFunction)},
			     {(char*) 0x0012, "WriteLine", (char*) 2, (char*) (*parseSystemFunction)},
			     {(char*) 0x0025, "WriteProfString", (char*) 4, (char*) (*parseSystemFunction)},

			     {(char*) 0x00fa, "XCopyFile", (char*) 3, (char*) (*parseSystemFunction)},


			     // arithmetic operations
			     {(char*) 0x0119, "+", (char*) OP_PLUS, (char*) (*decodeOp)},
			     {(char*) 0x0032, "+", (char*) OP_PLUS, (char*) (*singleVarOp)},
			     {(char*) 0x011a, "-", (char*) OP_MINUS, (char*) (*decodeOp)},    // X = Y - Z
			     {(char*) 0x0075, "-", (char*) OP_MINUS, (char*) (*singleVarOp)}, // X -= Y
			     {(char*) 0x011b, "*", (char*) OP_MULT, (char*) (*decodeOp)},
			     {(char*) 0x011c, "/", (char*) OP_DIV, (char*) (*decodeOp)},
			     {(char*) 0x0123, "%", (char*) OP_MOD, (char*) (*decodeOp)},

			     // binary operations
			     {(char*) 0x011d, "&", (char*) OP_BITAND, (char*) (*decodeOp)},
			     {(char*) 0x011e, "|", (char*) OP_BITOR, (char*) (*decodeOp)},
			     {(char*) 0x011f, "^", (char*) OP_BITEOR, (char*) (*decodeOp)},
			     {(char*) 0x0120, "~", (char*) OP_BITNOT, (char*) (*notOp)},

			     // shift operators
			     {(char*) 0x0121, "<<", (char*) OP_SHIFTL, (char*) (*decodeOp)},
			     {(char*) 0x0122, ">>", (char*) OP_SHIFTR, (char*) (*decodeOp)},

			     // logical operators
			     {(char*) 0x0127, "&&", (char*) OP_LOGICAND, (char*) (*decodeOp)},
			     {(char*) 0x0126, "||", (char*) OP_LOGICOR, (char*) (*decodeOp)},
			     // ! (NOT) is done by comparing with 0

			     // string operators
			     {(char*) 0x0124, "+", (char*) OP_STRCAT, (char*) (*decodeOp)},
			     {(char*) 0x0125, "^", (char*) OP_PATHCAT, (char*) (*decodeOp)},
			     // other ops are done with StringCompare() and 
			     // the numerical operators.

			     // copy commands (e.g. X = D)
			     {(char*) 0x0021, "=", NULL, (char*) (*equate)}, // number = number  (= Assign())
			     {(char*) 0x0013, "=", NULL, (char*) (*equate)}, // string = string (= StrCopy())

			     // number comparison operators
			     {(char*) 0x0128, "=", NULL, (char*) (*comparison)},

			     // function stuff
			     {(char*) 0x00b6, NULL, NULL, (char*) (*functionStart)},
			     {(char*) 0x00b8, NULL, NULL, (char*) (*functionEnd)},
			     {(char*) 0x00b7, NULL, NULL, (char*) (*funcReturn)},
			     {(char*) 0x012f, NULL, NULL, (char*) (*funcReturn)},

			     // flow control stuff
			     {(char*) 0x002b, "exit", NULL, (char*) (*doExit)},
			     {(char*) 0x0159, "abort", NULL, (char*) (*doAbort)},
			     {(char*) 0x00b5, NULL, NULL, (char*) (*parseUserFunction)},  // user function
			     {(char*) 0x0000, NULL, NULL, (char*) (*label)},
			     {(char*) 0x002c, NULL, NULL, (char*) (*doGoto)},
			     {(char*) 0x0043, NULL, NULL, (char*) (*doCall)},
			     {(char*) 0x0044, NULL, NULL, (char*) (*doReturn)},
			     {(char*) 0x0022, NULL, NULL, (char*) (*ifStmt)},
			     {(char*) 0x00b4, NULL, NULL, (char*) (*parseDLLFunction)},

			     {(char*) 0x0129, NULL, NULL, NULL}                // WHILE/SWITCH => ignore it

};

static int rawDecodeTableSize = sizeof(rawDecodeTable) / (sizeof(char*)*4);

typedef void (*decodeFunc)(int, int, int, ISData*);

typedef struct
{
  char* text;
  union
  {
    int numParams;
    int operationNumber;
  } info;
  decodeFunc decodeFunction0;
  decodeFunc decodeFunction1;
} DecodeEntry;

static DecodeEntry** decodeTable;

#define DECODETABLESIZE 512

#endif
