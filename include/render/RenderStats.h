#pragma once
namespace render 
{
	struct RenderStats 
	{
		int draws = 0;
		int quads = 0;
		void addDraws(int n = 1) { draws += n; }
		void addQuads(int n) { quads += n; }
		RenderStats & operator+=(const RenderStats & o) { draws += o.draws; quads += o.quads; return *this; }
	};
}