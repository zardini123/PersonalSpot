import unicodedata
import logging
from validate_email import validate_email

log = logging.getLogger("validation")

def rmControlChars(s):
    return "".join(ch for ch in s if unicodedata.category(ch)[0]!="C")

def containsControlChars(s):
    for ch in s:
        if unicodedata.category(ch)[0]=="C":
            return True
    return False

def validateUsername(name):
    if containsControlChars(name):
        log.warning("Username cannot contain Unicode control characters")
        return True
    return False

def validatePassword(password):
    if len(password) <= 6:
        log.warning("Password must be greater than 6 characters")
        return True
    elif len(password) >= 160:
        log.warning("Password must be less than 160 characters")
        return True
    elif containsControlChars(password):
        log.warning("Password cannot contain Unicode control characters")
        return True
    return False

def validateEmail(email):
    is_valid = False
    try:
        # Verify email by characters only, and not attempt to connect to SMTP Server
        is_valid = validate_email(email, verify=False)
    except Exception as e:
        log.error("Error verifying email", exc_info=True)

    return is_valid
