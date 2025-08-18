# python script/build_time.py inc/build_time.h
import time, sys, pathlib
sec = int(time.time())
pathlib.Path(sys.argv[1]).write_text(f"#ifndef BUILD_TIME\n#define BUILD_TIME {sec}U\n#endif")