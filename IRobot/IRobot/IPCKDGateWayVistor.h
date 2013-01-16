#pragma once

class IPCKDGateWayVistor
{
public:
	virtual BOOL Vistor() = 0;
	virtual BOOL ResultStrToTable(char *) = 0;
	virtual BOOL SendMsg(char *) = 0;
};
