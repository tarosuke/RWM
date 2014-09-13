#pragma once


class IMAGE{
public:
	IMAGE();
	IMAGE(const IMAGE&);
	IMAGE(const IMAGE&, int x, int y, unsigned w, unsigned h);
	IMAGE(const void*, unsigned w, unsigned h, unsigned bytesDepth);
	IMAGE(unsigned w, unsigned h, unsigned bytesDepth);

	virtual ~IMAGE();

	void Update(const IMAGE&, int x, int y);

	void operator=(const IMAGE&);

	unsigned Pixels() const{ return width * height; };
	unsigned Size() const{ return Pixels() * depth; };
	unsigned Width() const{ return width; };
	unsigned Height() const{ return height; };
	unsigned Depth() const{ return depth; };
	const void* Buffer() const{ return buffer ? buffer : constBuffer; };
	void* WritableBuffer() const{ return buffer; };
protected:
	void AssignBuffer(void*);
	void AssignBuffer(unsigned w, unsigned h, unsigned d);
	bool IsInRange(unsigned x, unsigned y) const;
	unsigned GetOffset(unsigned x, unsigned y) const;
	void* GetPoint(unsigned, unsigned);
	const void* GetConstPoint(unsigned, unsigned) const;
private:
	void* buffer;
	const void* constBuffer;
	unsigned width;
	unsigned height;
	unsigned depth;
};
