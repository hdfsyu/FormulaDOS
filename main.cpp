#include <iostream>
#include <string>
using namespace std;
#include "olcConsoleGameEngine.h"
class FormulaDOS : public olcConsoleGameEngine {
public:
	FormulaDOS() {
		m_sAppName = L"Formula DOS";
	}
private:
	float cp = 0.0f; // car pos
	float d = 0.0f;// distance
	float speed = 0.0f;
	float c = 0.0f; //curvature
	float pc = 0.0f; // player curvature
	float tc = 0.0f;
	float td = 0.0f;
	float clt = 0.0f;
	vector<pair<float, float>> track;
	list<float> lapTimes;
protected:
	virtual bool OnUserUpdate(float et) {
		int cd = 0; // car direction
		if (m_keys[VK_UP].bHeld) {
			cd = 0;
			speed += 4.0f * et;
		}
		else {
			cd = 0;
			speed -= 3.0f * et;
		}
		if (m_keys[VK_LEFT].bHeld) {
			pc -= 1.0f * et;
			cd = -1;
		}
		if (m_keys[VK_RIGHT].bHeld) {
			pc += 1.0f * et;
			cd = 1;
		}
		if (fabs(pc - tc) >= 1.1f) {
			speed -= 5.0f * et;
		}
		if (speed < 0.0f) speed = 0.0f;
		if (speed > 1.0f) speed = 1.0f;
		d += (150.0f * speed) * et;
		float offset = 0;
		int ts = 0; //track section (index of vector)
		clt += et;
		if (d >= td) {
			d -= td;
			lapTimes.push_front(clt);
			lapTimes.pop_back();
			clt = 0.0f;
		}
		while (ts < track.size() && offset <= d) {
			offset += track[ts].second;
			ts++;
		}
		float tc = track[ts - 1].first;// target curvature
		float tcd = (tc - c) * et * speed; // track curve difference
		c += tcd;
		tc = (c)*et * speed;
		for (int y = 0; y < ScreenHeight() / 2; y++) {
			for (int x = 0; x < ScreenWidth(); x++) {
				Draw(x, y, y < ScreenHeight() / 4 ? PIXEL_HALF : PIXEL_SOLID, FG_DARK_BLUE);
			}
		}
		for (int x = 0; x < ScreenWidth(); x++) {
			int hh = (int)(fabs(sinf(x * 0.01f + tc) * 16.0f));
			for (int y = (ScreenHeight() / 2) - hh; y < ScreenHeight() / 2; y++) {
				Draw(x, y, PIXEL_SOLID, FG_DARK_YELLOW);
			}
		}
		for (int y = 0; y < ScreenHeight() / 2; y++) {
			for (int x = 0; x < ScreenWidth(); x++) {
				float p = (float)y / (ScreenHeight() / 2.0f); // perspective
				float mp = 0.5f + c * powf((1.0f - p), 2);; //middle point
				float rw = 0.1f+p * 0.8; // road width
				float cw = rw * 0.15f; // clip width
				rw *= 0.5f;
				int lg = (mp - rw - cw) * ScreenWidth();
				int lc = (mp - rw) * ScreenWidth();
				int rg = (mp + rw + cw) * ScreenWidth();
				int rc = (mp + rw) * ScreenWidth();
				int r = ScreenHeight() / 2 + y;
				int gc = sinf(20.0f * powf(1.0f - p, 3) + d * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;//grass colour
				int cc = sinf(80.0f * powf(1.0f - p, 2) + d * 0.1f) > 0.0f ? FG_RED : FG_WHITE;//clip colour
				int roc = (ts - 1) == 0 ? FG_WHITE : FG_GREY;//road colour
				if (x >= 0 && x < lg) {
					Draw(x, r, PIXEL_SOLID, gc);
				}if (x >= lg && x < lc) {
					Draw(x, r, PIXEL_SOLID, cc);
				}if (x >= lc && x < rc) {
					Draw(x, r, PIXEL_SOLID, roc);
				}if (x >= rc && x < rg) {
					Draw(x, r, PIXEL_SOLID, cc);
				}if (x >= rg && x < ScreenWidth()) {
					Draw(x, r, PIXEL_SOLID, gc);
				}
			}
		}
		cp = pc - tc;
		int ncp = ScreenWidth() / 2 + ((int)(ScreenWidth() * cp) / 2.0f) - 7;// carpos integer
		switch (cd)
		{
		case 0:
			DrawStringAlpha(ncp, 80, L"!!!######!!!");
			DrawStringAlpha(ncp, 81, L"![-######-]!");
			DrawStringAlpha(ncp, 82, L"!!!######!!!");
			DrawStringAlpha(ncp, 83, L"    ####    ");
			DrawStringAlpha(ncp, 84, L"    [##]    ");
			DrawStringAlpha(ncp, 85, L"    ####    ");
			DrawStringAlpha(ncp, 86, L" |||####||| ");
		case +1:
			DrawStringAlpha(ncp, 80, L"   ///######///");
			DrawStringAlpha(ncp, 81, L"  /[-######-]/");
			DrawStringAlpha(ncp, 82, L" ///######///");
			DrawStringAlpha(ncp, 83, L"     ####   ");
			DrawStringAlpha(ncp, 84, L"    [##]    ");
			DrawStringAlpha(ncp, 85, L"   ####     ");
			DrawStringAlpha(ncp, 86, L"|||####|||  ");
		case -1:
			DrawStringAlpha(ncp, 80, L"\\\\\\######\\\\\\");
			DrawStringAlpha(ncp, 81, L" \\[-######-]\\");
			DrawStringAlpha(ncp, 82, L"  \\\\\\######\\\\\\");
			DrawStringAlpha(ncp, 83, L"   ####     ");
			DrawStringAlpha(ncp, 84, L"    [##]    ");
			DrawStringAlpha(ncp, 85, L"     ####   ");
			DrawStringAlpha(ncp, 86, L"  |||####|||");
		}
		DrawString(0, 0, L"Distance: " + to_wstring(d));
		DrawString(0, 1, L"Target Curvature: " + to_wstring(c));
		DrawString(0, 2, L"Player Curvature: " + to_wstring(pc));
		DrawString(0, 3, L"Track Curvature: " + to_wstring(tc));
		DrawString(0, 4, L"Player Speed: " + to_wstring(speed));
		auto disp_time = [](float t) {
			int m = t / 60.0f;
			int s = t - (m * 60.0f);
			int ms = (t - (float)s) * 1000.0f;
			return to_wstring(m) + L"." + to_wstring(s) + L":" + to_wstring(ms);
		};
		DrawString(10, 8, disp_time(clt));
		int j = 10;
		for (auto l : lapTimes) {
			DrawString(10, j, disp_time(l));
			j++;
		}
		return true;
	}
	virtual bool OnUserCreate() {
		// curvature, then units (units are meters)
		track.push_back(make_pair(0.0f, 10.0f));//start/finish line
		track.push_back(make_pair(0.0f, 200.0f));
		track.push_back(make_pair(1.0f, 200.0f));
		track.push_back(make_pair(0.0f, 400.0f));
		track.push_back(make_pair(-1.0f, 100.0f));
		track.push_back(make_pair(0.0f, 200.0f));
		track.push_back(make_pair(-1.0f, 200.0f));
		track.push_back(make_pair(1.0f, 200.0f));
		track.push_back(make_pair(0.0f, 200.0f));
		track.push_back(make_pair(0.2f, 500.0f));
		track.push_back(make_pair(0.0f, 200.0f));
		for (auto t : track) {
			td += t.second;
		}
		lapTimes = { 0,0,0,0,0 };
		return true;
	}
};
int main() {
	FormulaDOS game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	return 0;
}