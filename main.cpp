/* File: main.cpp
 * Author: CRE
 * Last Edited: Wed Nov  9 17:13:32 2016
 */

#include "vcfdivider.h"
#include <iostream>
using namespace std;
using namespace cre;

static inline void printHelp()
{
	die("Usage:\n"\
			"\tvcfdivider vcffile.vcf selectsamplefile\n"\
			"selectedsamplefile:\ns1\ns2\n...\n"\
			"Output:vcffile.Selected.vcf & vcffile.Deleted.vcf\n"
			);
}

int main (int argc , char ** argv)
{
	if (argc!=3) printHelp();

	vcfdivide(argv[1], argv[2]);
	return 0;
}
