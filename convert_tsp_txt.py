import sys
import os

EXCLUDED = [
	'NAME',
	'COMMENT',
	'TYPE',
	'DIMENSION:',
	'DIMENSION',
	'EDGE_WEIGHT_TYPE',
	'NODE_COORD_SECTION',
	'EOF',
	'COMMENT:',
	'TYPE:',
	'NAME:',
	'EOF',
	'EDGE_WEIGHT_TYPE:',
	'DISPLAY_DATA_TYPE:',
	'EDGE_WEIGHT_FORMAT:',
	'EOF',
]

for tsp in os.listdir(os.getcwd()):
	if os.path.splitext(tsp)[1] in '.tsp':
		infile = open(os.path.join(os.getcwd(), tsp), 'r')
		outfile = open(os.path.join(os.getcwd(), tsp.replace('.tsp', '.txt')), 'w')
		nodeCoordinate = False
		for line in infile:
			buffer = line.split();
			if len(buffer)>0 and buffer[0] == 'NODE_COORD_SECTION':
				nodeCoordinate = True
			if len(buffer)>0 and buffer[0] not in EXCLUDED:
				outfile.write(line)
		if not nodeCoordinate:
			print tsp
		# sys.exit(0);