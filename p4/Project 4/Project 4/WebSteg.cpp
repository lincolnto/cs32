#include "provided.h"
#include "http.h"
#include <string>
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
	string pageContents;
	HTTP().get(url, pageContents);
	if (!Steg::hide(pageContents, msg, host)) return false;
	return true;  // This compiles, but may not be correct
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
	string pageContents;
	HTTP().get(url, pageContents);
	if (!Steg::reveal(pageContents, msg)) return false;

	return true;
}
