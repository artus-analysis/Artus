


class CustomException(Exception):
	"""
	Base class for custom exceptions
	"""
	__init__(self, *args, **kwargs):
		Exception.__init__(self, *args, **kwargs)

class UserError(CustomException):
	"""
	To be raised when user/config input is inadequate
	"""
	pass

class DynamicContentError(CustomException):
	"""
	To be raised when requested dynamically loaded content is not available
	"""
	pass

# NotImplementedError (from inbuilt exceptions)
#   In user defined base classes, abstract methods should raise this exception when they require derived classes to override the method.

class ToBeImplementedError(CustomException):
	"""
	In (derived) classes, methods should raise this exception when 
	"""
	__init__(self, *args, **kwargs):
		Exception.__init__(self, *args, **kwargs)
		
class NotApplicableError(CustomException):
	"""
	In (derived) classes, methods should raise this exception when it is logically impossible to provide functionality which is very likely to be expected
	"""
	__init__(self, *args, **kwargs):
		Exception.__init__(self, *args, **kwargs)
