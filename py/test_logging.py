import logging
from logging.handlers import TimedRotatingFileHandler
import subprocess

#cmd = ["echo '123'"]
#fild = TimedRotatingFileHandler("123", when = "H", interval = 2)
#subprocess.Popen(cmd, stdout=file, stderr=file)
logger = logging.getLogger()
logger.debug("123")
logger.info("123")
logger.warning("123")
logger.error("123")
