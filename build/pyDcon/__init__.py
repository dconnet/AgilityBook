import os

from . import RmMinusRF
from . import Run

if os.name == 'nt':
	from . import GetVSDir
	from . import VSPaths
