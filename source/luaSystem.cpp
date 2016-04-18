/*----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#------  This File is Part Of : ----------------------------------------------------------------------------------------#
#------- _  -------------------  ______   _   --------------------------------------------------------------------------#
#------ | | ------------------- (_____ \ | |  --------------------------------------------------------------------------#
#------ | | ---  _   _   ____    _____) )| |  ____  _   _   ____   ____   ----------------------------------------------#
#------ | | --- | | | | / _  |  |  ____/ | | / _  || | | | / _  ) / ___)  ----------------------------------------------#
#------ | |_____| |_| |( ( | |  | |      | |( ( | || |_| |( (/ / | |  --------------------------------------------------#
#------ |_______)\____| \_||_|  |_|      |_| \_||_| \__  | \____)|_|  --------------------------------------------------#
#------------------------------------------------- (____/  -------------------------------------------------------------#
#------------------------   ______   _   -------------------------------------------------------------------------------#
#------------------------  (_____ \ | |  -------------------------------------------------------------------------------#
#------------------------   _____) )| | _   _   ___   ------------------------------------------------------------------#
#------------------------  |  ____/ | || | | | /___)  ------------------------------------------------------------------#
#------------------------  | |      | || |_| ||___ |  ------------------------------------------------------------------#
#------------------------  |_|      |_| \____|(___/   ------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Licensed under the GPL License --------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Copyright (c) Nanni <lpp.nanni@gmail.com> ---------------------------------------------------------------------------#
#- Copyright (c) Rinnegatamante <rinnegatamante@gmail.com> -------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Credits : -----------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- All the devs involved in Rejuvenate and vita-toolchain --------------------------------------------------------------#
#- xerpi for drawing libs and for FTP server code ----------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/power.h>
#include <psp2/appmgr.h>
#include <psp2/apputil.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/system_param.h>
#include <psp2/io/dirent.h>
#include <psp2/rtc.h>
#include <psp2/uvl.h>
#include "include/luaplayer.h"
#include "main_menu.cpp"

#define stringify(str) #str
#define VariableRegister(lua, value) do { lua_pushinteger(lua, value); lua_setglobal (lua, stringify(value)); } while(0)

int FREAD = SCE_O_RDONLY;
int FWRITE = SCE_O_WRONLY;
int FCREATE = SCE_O_CREAT | SCE_O_WRONLY;
int FRDWR = SCE_O_RDWR;
int SET = 1;
int CUR = 2;
int END = 3;
extern int script_files;

static char *lang[] ={//Lang
	"japanese",
	"english_us",
	"french",
	"spanish",
	"german",
	"italian",
	"dutch",
	"portuguese",
	"russian",
	"korean",
	"chinese_t",
	"chinese_s",
	"finnish",
	"swedish",
	"danish",
	"norwegian",
	"polish",
	"portuguese_br",
	"english_gb"
};

/*
	getSizeString() by TheFlow modified by Applelo 
	Copyright (C) 2015-2016, TheFloW
*/
void getSizeString(char *string, uint64_t size) {
	double double_size = (double)size;

	int i = 0;
	while (double_size >= 1024.0f) {
		double_size /= 1024.0f;
		i++;
	}

	sprintf(string, "%.*f", (i == 0) ? 0 : 2, double_size);
}
//////////////////////////////////////


static int lua_dofile(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "dofile() takes one argument.");
	char* file = (char*)luaL_checkstring(L,1);
	unsigned char* buffer;
	SceUID script = sceIoOpen(file, SCE_O_RDONLY, 0777);
	if (script < 0) return luaL_error(L, "error opening file.");
	else{
		SceOff size = sceIoLseek(script, 0, SEEK_END);
		sceIoLseek(script, 0, SEEK_SET);
		buffer = (unsigned char*)malloc(size + 1);
		sceIoRead(script, buffer, size);
		buffer[size] = 0;
		sceIoClose(script);
	}
	lua_settop(L, 1);
	if (luaL_loadbuffer(L, (const char*)buffer, strlen((const char*)buffer), NULL) != LUA_OK)	return lua_error(L);
	lua_KFunction dofilecont = (lua_KFunction)(lua_gettop(L) - 1);
	lua_callk(L, 0, LUA_MULTRET, 0, dofilecont);
	return (int)dofilecont;
}

static int lua_openfile(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) return luaL_error(L, "wrong number of arguments");
	const char *file_tbo = luaL_checkstring(L, 1);
	int type = luaL_checkinteger(L, 2);
	SceUID fileHandle = sceIoOpen(file_tbo, type, 0777);
	lua_pushinteger(L,fileHandle);
	return 1;
}

static int lua_readfile(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 2) return luaL_error(L, "wrong number of arguments");
	int file = luaL_checkinteger(L, 1);
	int size = luaL_checkinteger(L, 2);
	unsigned char *buffer = (unsigned char*)(malloc((size+1) * sizeof (char)));
	sceIoRead(file,buffer, size);
	buffer[size] = 0;
	lua_pushlstring(L,(const char*)buffer,size);
	free(buffer);
	return 1;
}

static int lua_writefile(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) return luaL_error(L, "wrong number of arguments");
	int fileHandle = luaL_checkinteger(L, 1);
	const char *text = luaL_checkstring(L, 2);
	int size = luaL_checknumber(L, 3);
	sceIoWrite(fileHandle, text, size);
	return 0;
}

static int lua_closefile(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	int fileHandle = luaL_checkinteger(L, 1);
	sceIoClose(fileHandle);
	return 0;
}

static int lua_seekfile(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 3) return luaL_error(L, "wrong number of arguments");
	int fileHandle = luaL_checkinteger(L, 1);
	int pos = luaL_checkinteger(L, 2);
	int type = luaL_checkinteger(L, 3);
	sceIoLseek(fileHandle, pos, type);	
	return 0;
}

static int lua_sizefile(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	int fileHandle = luaL_checkinteger(L, 1);
	SceOff size = sceIoLseek(fileHandle, 0, SEEK_END);
	sceIoLseek(fileHandle, 0, SEEK_SET);
	lua_pushinteger(L, size);
	return 1;
}

static int lua_checkexist(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	const char *file_tbo = luaL_checkstring(L, 1);
	SceUID fileHandle = sceIoOpen(file_tbo, SCE_O_RDONLY, 0777);
	if (fileHandle < 0) lua_pushboolean(L, false);
	else{
		sceIoClose(fileHandle);
		lua_pushboolean(L,true);
	}
	return 1;
}

static int lua_rename(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	const char *old_file = luaL_checkstring(L, 1);
	const char *new_file = luaL_checkstring(L, 2);
	sceIoRename(old_file, new_file);
	return 0;
}

static int lua_removef(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	const char *old_file = luaL_checkstring(L, 1);
	sceIoRemove(old_file);
	return 0;
}

static int lua_removef2(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	const char *old_file = luaL_checkstring(L, 1);
	sceIoRmdir(old_file);
	return 0;
}

static int lua_newdir(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	const char *newdir = luaL_checkstring(L, 1);
	sceIoMkdir(newdir, 0777);
	return 0;
}


static int lua_exit(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0 && argc != 1) return luaL_error(L, "System.exit() takes 1 or no arguments.");
	extern int goodbye;
	if (argc == 0) goodbye = 0;
	else goodbye = luaL_checkinteger(L, 1);
	
	if (goodbye!= 0 && goodbye != 1) {
		goodbye = 2;
		return luaL_error(L, "invalid arguments");
	}
	
    luaL_dostring(L, "collectgarbage()");
    return luaL_error(L,"fakerror"); //Fake LUA error
}

static int lua_wait(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "System.wait() takes no arguments.");
	int microsecs = luaL_checkinteger(L, 1);
	sceKernelDelayThread(microsecs);
	return 0;
}


SceIoDirent g_dir;

static int lua_dir(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0 && argc != 1) return luaL_error(L, "System.listDirectory([path]) takes zero or one argument");


    const char *path = "";
    if (argc == 0) {
        path = "";
    } else {
        path = luaL_checkstring(L, 1);
    }
    int fd = sceIoDopen(path);

    if (fd < 0) {
        lua_pushnil(L);  /* return nil */
        return 1;
    }
    lua_newtable(L);
    int i = 1;
    while (sceIoDread(fd, &g_dir) > 0) {
        lua_pushnumber(L, i++);  /* push key for file entry */

        lua_newtable(L);
            lua_pushstring(L, "name");
            lua_pushstring(L, g_dir.d_name);
            lua_settable(L, -3);

            lua_pushstring(L, "size");
            lua_pushnumber(L, g_dir.d_stat.st_size);
            lua_settable(L, -3);

            lua_pushstring(L, "directory");
            lua_pushboolean(L, SCE_S_ISDIR(g_dir.d_stat.st_mode));
            lua_settable(L, -3);

        lua_settable(L, -3);
    }

    sceIoDclose(fd);

    return 1;  /* table is already on top */
}

static int lua_charging(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) return luaL_error(L, "System.isBatteryCharging() takes no arguments.");
	lua_pushboolean(L, scePowerIsBatteryCharging());
	return 1;
}

static int lua_percent(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) return luaL_error(L, "System.getBatteryPercentage() takes no arguments.");
	lua_pushinteger(L, scePowerGetBatteryLifePercent());
	return 1;
}

static int lua_lifetime(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) return luaL_error(L, "System.getBatteryLifetime() takes no arguments.");
	lua_pushinteger(L, scePowerGetBatteryLifeTime());
	return 1;
}


static int lua_nopower(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) return luaL_error(L, "System.powerTick() takes no arguments.");
	sceKernelPowerTick(0);
	return 0;
}

static int lua_loadhb(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 1) return luaL_error(L, "System.loadElf() takes one argument.");
	char* path = (char*)luaL_checkstring(L, 1);
	luaL_dostring(L, "collectgarbage()");
	uvl_load(path);
	return 0;
}

static int lua_size(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) return luaL_error(L, "System.getSize() takes no arguments.");
	uint64_t free_size = 0, max_size = 0;
	sceAppMgrGetDevInfo("ux0:", &max_size, &free_size);
	char free_size_string[16], max_size_string[16];
	double double_size = (double)free_size;
	int i = 0;
	while (double_size >= 1024.0f) {
		double_size /= 1024.0f;
		i++;
	}
	sprintf(free_size_string, "%.*f", (i == 0) ? 0 : 2, double_size);
	i=0;
	double_size = (double)max_size;
	while (double_size >= 1024.0f) {
		double_size /= 1024.0f;
		i++;
	}
	sprintf(max_size_string, "%.*f", (i == 0) ? 0 : 2, double_size);
	lua_pushstring(L,max_size_string);
	lua_pushstring(L,free_size_string);
	return 2;
}

static int lua_language(lua_State *L)
	{

    if (lua_gettop(L) != 0) return luaL_error(L, "System.getLanguage() takes no arguments.");
	int language;

	// Init SceAppUtil
	SceAppUtilInitParam init_param;
	SceAppUtilBootParam boot_param;
	memset(&init_param, 0, sizeof(SceAppUtilInitParam));
	memset(&boot_param, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&init_param, &boot_param);
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &language);
	// Shutdown AppUtil
	sceAppUtilShutdown();
	lua_pushstring(L,lang[language]);

	return 1;
}

static int lua_mac(lua_State *L)
{
    if (lua_gettop(L) != 0) return luaL_error(L,"System.getMacAddress() takes no arguments.");
	static char memory[16 * 1024];
	SceNetInitParam param;
		param.memory = memory;
		param.size = sizeof(memory);
		param.flags = 0;
	SceNetEtherAddr mac;//Mac adress
	int net_init = sceNetInit(&param);
	int netctl_init = sceNetCtlInit();
	// Get mac address
	sceNetGetMacAddress(&mac, 0);
	if (netctl_init >= 0) sceNetCtlTerm();
	if (net_init >= 0) sceNetTerm();
	
	char mac_string[32];
	sprintf(mac_string, "%02X:%02X:%02X:%02X:%02X:%02X", mac.data[0], mac.data[1], mac.data[2], mac.data[3], mac.data[4], mac.data[5]);
	
	lua_pushstring(L,mac_string);
	return 1;
}

static int lua_ip(lua_State *L)
{
    if (lua_gettop(L) != 0) return luaL_error(L, "System.getIpAddress() takes no arguments.");
	static char memory[16 * 1024];
	SceNetInitParam param;
		param.memory = memory;
		param.size = sizeof(memory);
		param.flags = 0;
	char ip[16];
	int net_init = sceNetInit(&param);
	int netctl_init = sceNetCtlInit();
	
	// Get IP
	SceNetCtlInfo info;
	if (sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &info) < 0) {
		strcpy(ip, "-");
	} else {
		strcpy(ip, info.ip_address);
	}
	
	if (netctl_init >= 0) sceNetCtlTerm();
	if (net_init >= 0) sceNetTerm();
	lua_pushstring(L,ip);
	return 1;
}

static int lua_time(lua_State *L)
{
    if (lua_gettop(L) != 0) return luaL_error(L, "System.getTime() takes no arguments.");
    
    SceRtcTime vitatime;
    sceRtcGetCurrentClockLocalTime(&vitatime);

    lua_newtable(L);

    lua_pushstring(L, "year");
    lua_pushinteger(L,vitatime.year);
    lua_settable(L, -3);
	
    lua_pushstring(L, "month");
    lua_pushinteger(L,vitatime.month);
    lua_settable(L, -3);

    lua_pushstring(L, "day");
    lua_pushinteger(L,vitatime.day);
    lua_settable(L, -3);
	
    lua_pushstring(L, "hour");
    lua_pushinteger(L,vitatime.hour);
    lua_settable(L, -3);

    lua_pushstring(L, "minutes");
    lua_pushinteger(L,vitatime.minutes);
    lua_settable(L, -3);

    lua_pushstring(L, "seconds");
    lua_pushinteger(L,vitatime.seconds);
    lua_settable(L, -3);

    lua_pushstring(L, "microseconds");
    lua_pushinteger(L,vitatime.microseconds);
    lua_settable(L, -3);

    return 1;
}


/*		[[[[[[[WIP]]]]]]]]]
static int lua_curdir(lua_State *L)
{
    int argc = lua_gettop(L);
    if(argc != 0 && argc != 1) return luaL_error(L, "System.currentDir([Path]) takes 1 or no arguments.");
    if(argc == 1) chdir(luaL_checkstring(L, 1));
    char path[256] = "";
	lua_pushstring(L,getcwd(path, 256));
    return(1);
}

static int lua_versionvita(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) return luaL_error(L, "wrong number of arguments");
	SwVersionParam sw_ver_param;
	sw_ver_param.size = sizeof(SwVersionParam);
	sceKernelGetSystemSwVersion(&sw_ver_param);
	lua_pushstring(L,sw_ver_param.version_string);
	return 1;
}

static int lua_modelvita(lua_State *L)
{
    int argc = lua_gettop(L);
    if (argc != 0) return luaL_error(L, "wrong number of arguments");
	char model_string[10];
	sprintf(model_string,"0x%08X",sceKernelGetModelForCDialog());
	//sceKernelGetModelForCDialog()
	lua_pushstring(L,model_string);
	return 1;
}
*/



//Register our System Functions
static const luaL_Reg System_functions[] = {

  // Dofile & I/O Library patched functions
  {"doNotUse",							lua_dofile},
  {"doNotOpen",							lua_openfile},
  {"doNotRead",							lua_readfile},
  {"doNotWrite",						lua_writefile},
  {"doNotClose",						lua_closefile},  
  {"doNotSeek",							lua_seekfile},  
  {"doNotSize",							lua_sizefile},  
  
  {"doesFileExist",						lua_checkexist},
  {"exit",								lua_exit},
  {"rename",							lua_rename},
  {"deleteFile",						lua_removef},
  {"deleteDirectory",					lua_removef2},
  {"createDirectory",					lua_newdir},
  //{"currentDirectory",					lua_curdir},
  {"listDirectory",						lua_dir},
  {"wait",								lua_wait},
  {"isBatteryCharging",					lua_charging},
  {"getBatteryPercentage",				lua_percent},
  {"getBatteryLifetime",				lua_lifetime},
  {"powerTick",							lua_nopower},
  {"loadElf",							lua_loadhb},
  {"getLanguage",						lua_language},
  {"getSize",							lua_size},
  {"getMacAddress",						lua_mac},
  {"getIpAddress",						lua_ip},
  {"getTime",							lua_time},
 //{"getVersion",						lua_versionvita},
  //{"getModel",							lua_modelvita},
  {0, 0}
  
};

void luaSystem_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, System_functions, 0);
	lua_setglobal(L, "System");
	VariableRegister(L,FREAD);
	VariableRegister(L,FWRITE);
	VariableRegister(L,FCREATE);
	VariableRegister(L,FRDWR);
	VariableRegister(L,SET);
	VariableRegister(L,END);
	VariableRegister(L,CUR);
}