#include <stdio.h>
#include <windows.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define targetPos 5 //Which specific Position struct to test.
#define bufferSize 10

typedef struct position{
    int x;
    int y;
    int z;
}Position;

int setX(lua_State* L) //Setter function for X from Lua, must have this signature
{
    Position* pos = (Position*)lua_touserdata(L, lua_upvalueindex(1)); //Grabs struct from upvalue bound to function
    pos->x = (int)lua_tonumber(L, -1); //Grabs argument from Lua off the Lua stack. It also pops it off the stack if I remember right.
    return 0; //Returns the number of things put onto the Lua stack during the functions, 0 in this case
}

int setY(lua_State* L)
{
    Position* pos = (Position*)lua_touserdata(L, lua_upvalueindex(1));
    pos->y = (int)lua_tonumber(L, -1);
    return 0;
}

int setZ(lua_State* L)
{
    Position* pos = (Position*)lua_touserdata(L, lua_upvalueindex(1));
    pos->z = (int)lua_tonumber(L, -1);
    return 0;
}

int getPositions(lua_State* L)
{
    printf("Grabbing Positions!\n");
    Position pos = *(Position*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushnumber(L, pos.x);
    lua_pushnumber(L, pos.y);
    lua_pushnumber(L, pos.z);
    return 3; //Lua lets you return multiple values from the same function, 3 in this case.
}

int printInt(lua_State* L)
{
    lua_Number num = lua_tonumber(L, -1);
    printf("Printing int from Lua: %i\n", (int)num);
    return 0;
}

void setupStates(lua_State** LBuffer, Position* positionbuffer, int pos)
{
    
    lua_State* L = luaL_newstate(); //The individual active state of each entity. These would share things such as global variables inside of each one, so u would suggest each entity gets one to avoid collisions and make scripting easy.
    LBuffer[pos] = L; //L is the naming cnvention for this, do not ask me why.
    lua_pushlightuserdata(L,  positionbuffer + pos); //Lightuserdata is managed by C rather than Lua so it does not get garbage collected. Specifically this is assigning each LuaState it's own position struct that is tied to it.
    lua_pushcclosure(L, getPositions, 1); //The closure is in effect a function. It also takes the previous number of positions on the Luastack equal to the last argument and sets them as upvalues bound to the function. Also pops them off the luastack.
    lua_setglobal(L, "getPositions"); //Binds the previous value as a global. Also pops it off the stack.

    lua_pushlightuserdata(L,  positionbuffer + pos);
    lua_pushcclosure(L, setX, 1);
    lua_setglobal(L, "setX");

    lua_pushlightuserdata(L,  positionbuffer + pos);
    lua_pushcclosure(L, setY, 1);
    lua_setglobal(L, "setY");

    lua_pushlightuserdata(L,  positionbuffer + pos);
    lua_pushcclosure(L, setZ, 1);
    lua_setglobal(L, "setZ");

    lua_pushcfunction(L, printInt); //The equivalent of lua_pushcclosure(L, printInt, 0)
    lua_setglobal(L, "printInt");

}

int main()
{
    Position positionbuffer[bufferSize];
    lua_State* Ls[bufferSize];

    printf("Hello World!\n");
    for (int i = 0; i < bufferSize; i++)
    {
        positionbuffer[i].x = i;
        positionbuffer[i].y = i + bufferSize;
        positionbuffer[i].z = i +  (bufferSize*2);
        setupStates(Ls, positionbuffer, i);
    }
    printf("Doing test for position: %i\n", targetPos);
    printf("X before Lua: %i\n", positionbuffer[targetPos].x);
    lua_State* L = Ls[targetPos];
    
    luaL_dostring(L, "x,y,z = getPositions()\nprintInt(x)\nprintInt(y)\nprintInt(z)\nsetX(42069)\nsetY(x+y+z)\nsetZ(z-y)"); //Executes the string as Lua code. There is a function to directly run a file instead.
    printf("X after Lua: %i\n", positionbuffer[targetPos].x);
    printf("Y after Lua: %i\n", positionbuffer[targetPos].y);
    printf("Z after Lua: %i\n", positionbuffer[targetPos].z);
}