#pragma once

#define BUF_SIZE 1024

class MsgNode
{
	using BufType = char;
	friend class Secsion;
public:
	MsgNode(void* msg, size_t bytes)
	{
		CreateDataBuf(bytes);
		memcpy(_data, msg, _total_len * sizeof(BufType));
	}
	MsgNode(size_t bytes)
	{
		CreateDataBuf(bytes);
	}

	MsgNode()
	{
		CreateDataBuf(BUF_SIZE);
	}
	~MsgNode()
	{
		delete[] _data;
	}
public:
	void Clear()
	{
		memset(_data, 0, _total_len * sizeof(BufType));
	}

private:
	void CreateDataBuf(size_t bytes)
	{
		if (bytes == 0)
			bytes = BUF_SIZE;

		_total_len = bytes;
		_data = new BufType[_total_len];
		memset(_data, 0, _total_len * sizeof(BufType));
		_transf_len = 0;
	}

private:
	BufType* _data;
	size_t _total_len;
	size_t _transf_len;
};

