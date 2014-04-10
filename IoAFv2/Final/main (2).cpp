#include "sql.h"

void main(int argc, char ** argv)
{
	REGQUEUE q;
	parser(argc, argv, &q);

	reg2sql(&q);
		
}