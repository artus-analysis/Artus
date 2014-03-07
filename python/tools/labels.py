import copy
from Artus.Plotting.tools.dictionaries import d_axes, d_plots


def labels(ax, opt, settings, subplot=False):
	"""This function prints all labels and captions in a plot.

	Several functions are called for each type of label.
	"""
	if not (settings['ratio'] and settings['subplot']
			and not settings['fit'] == 'intercept'):
#		if settings['lumi'] is not None and not settings['mconly']:
#			lumilabel(ax, settings['lumi'])  # always (if given) pure MC plots?
		statuslabel(ax, settings['status'])
		if settings['energy'] is not None:
			energylabel(ax, settings['energy'])
		#if jet==True:  jetlabel(ax, changes, sub_plot)	# on demand
		#if changes.has_key('var') or changes.has_key('bin'):
		#	binlabel(ax, bin, changes=changes, color=color)
		#if 'incut' in changes: incutlabel(ax, color, changes['incut'])
		#resultlabel(ax, result)
		authorlabel(ax, opt.author)
		datelabel(ax, opt.date)
		#if settings.get('eventnumberlabel', False) is True:
		#	plotbase.eventnumberlabel(ax, settings)
		#if settings['run'] is True:
		#	plotbase.runlabel(ax, settings)
		if 'selection' in opt.user_options:
			ax.text(0.98, 0.80, opt.user_options['selection'], va='top', ha='right',
							transform=ax.transAxes, size='small', color='black')
		if settings['subtext'] is not None:
			ax.text(-0.03, 1.01, settings['subtext'], va='bottom', ha='right',
					transform=ax.transAxes, size='xx-large', color='black')
#		if settings['extrapolation'] is not False:
#			ax.text(0.02, 0.02, "%s extrapolation applied" % settings['extrapolation'],
#				va='bottom', ha='left', transform=ax.transAxes, size='small',
#				color='black')
		if settings['text'] is not None:
			textlabel(ax, settings['text'])
		if settings.get('cutlabel', None) is not None:
			cutlabel(ax, settings)
	if type(settings['legloc']) == str:
		if "," in settings['legloc']:
			settings['legloc'] = ([float(i) for i in settings['legloc'].split(",")])
		legend = ax.legend(loc=settings['legloc'], numpoints=1, fancybox=True,
																	shadow=False)
	if settings['subtext'] is not None:
		ax.text(-0.04, 1.01, settings['subtext'], va='bottom', ha='right',
			transform=ax.transAxes, size='xx-large', color='black')
	return ax


def runlabel(ax, settings):
	runs = [['2012A', 190456.], ['2012B', 193834.],
		['2012C', 197770.], ['2012D', 203773.]]
	for [runlabel, runnumber] in runs:
		ax.axvline(runnumber, color='gray', linestyle='--', alpha=0.2)
		ax.text(
			(runnumber - settings['x'][0]) / (settings['x'][1] - settings['x'][0]),
			0.92, runlabel, transform=ax.transAxes, va='top',
			ha='left', color='gray', alpha=0.5, size='medium')


def textlabel(ax, text, x=0.02, y=0.02):
	if len(text.split(",")) is 3:
		text, x, y = text.split(",")
	ax.text(x, y, text, va='bottom', ha='left', transform=ax.transAxes,
		size='small', color='black')


def incutlabel(ax, color='black', incut=''):
	if incut == 'allevents':
		text = r"(before cuts)"
	elif incut == 'zcutsonly':
		text = r"(only $\mu$ and Z cuts applied)"
	elif incut == 'alleta':
		text = r"(jet 1 all $\eta$)"
	else:
		return
	ax.text(0.97, 0.97, text, va='top', ha='right', transform=ax.transAxes,
		color=color)
	return ax

cutlabeldict = {
	'pteta': r"$p_\mathrm{T}^\mathrm{Z}>30\ \mathrm{GeV}  \quad |\eta^\mathrm{Jet1}|<1.3$",
	'ptetaalpha': r"$p_\mathrm{T}^\mathrm{Z}>30\ \mathrm{GeV}  \quad |\eta^\mathrm{Jet1}|<1.3  \quad  \alpha<0.2$",
	'ptalpha': r"$p_\mathrm{T}^\mathrm{Z}>30\ \mathrm{GeV}  \quad  \alpha<0.2$",
	'ptetaalpha03': r"$p_\mathrm{T}^\mathrm{Z}>30\ \mathrm{GeV}  \quad |\eta^\mathrm{Jet1}|<1.3  \quad  \alpha<0.3$",
}


def cutlabel(ax, settings):
	if 'cutlabel' not in settings or (hasattr(ax, 'cutlabel') and ax.cutlabel == True):
		return
	text = cutlabeldict.get(settings['cutlabel'], False)
	if text:
		print "ASDFFSAD", text, settings.get('cutlabeloffset')
		ax.text(0.97, 0.80 - settings.get('cutlabeloffset', 0), text, va='top', ha='right', color='black', transform=ax.transAxes, size='large')
		ax.cutlabel = True


def eventnumberlabel(ax, settings):
	if 'events' not in settings:
		return
	text = ""
	for f, l in zip(settings['events'], settings['labels']):
		text += "\n %s Events: %1.1e" % (l, f)
	ax.text(0.7, 1.01, text, size='xx-small', va='bottom', ha='right',
			transform=ax.transAxes)


def lumilabel(ax, lumi=0.0, xpos=0.00, ypos=1.01):
	if (hasattr(ax, 'lumilabel') and ax.lumilabel == True):
		return
	if (hasattr(ax, 'number') and ax.number != 2) or (not hasattr(ax, 'number')):
		if lumi >= 1.0:
			ax.text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{fb}^{-1}$" %
				(lumi), va='bottom', ha='left', transform=ax.transAxes)
		elif lumi > 0.0:
			ax.text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{pb}^{-1}$" %
				(lumi * 1000.0), va='bottom', ha='left', transform=ax.transAxes)
		ax.lumilabel = True
	return ax


def energylabel(ax, energy, xpos=1.00, ypos=1.01):
	if (hasattr(ax, 'energylabel') and ax.energylabel == True) or (hasattr(ax, 'number') and ax.number == 2):
		return
	if energy is not None:
		ax.text(xpos, ypos, r"$\sqrt{s} = %u\,\mathrm{TeV}$" % (energy),
			va='bottom', ha='right', transform=ax.transAxes)
		ax.energylabel = True


def jetlabel_string(changes, opt):
	if 'algorithm' in changes:
		algorithm = changes['algorithm']
	else:
		algorithm = opt.algorithm
	if 'correction' in changes:
		correction = changes['correction']
	else:
		correction = opt.correction
	if "L1L2L3Res" in correction:
		corr = r"L1L2L3 Residual corrected"
	elif "L1L2L3" in correction:
		corr = r"L1L2L3 corrected"
	elif "L1L2" in correction:
		corr = r"L1L2 corrected"
	elif "L1" in correction:
		corr = r"L1 corrected"
	else:
		corr = r"uncorrected"
	if "ak5pfjets" in algorithm.lower():
		jet = r"Anti-$k_{T}$ 0.5 PF jets"
	elif "ak7pfjets" in algorithm.lower():
		jet = r"Anti-$k_{T}$ 0.7 PF jets"
	else:
		jet = ""
		corr = ""
	return (jet, corr)


def jetlabel(ax, changes={}, sub_plot=False, posx=0.05, posy=0.95, opt=None):
#	res = jetlabel_string(changes, opt)
	res = ""
	if sub_plot:
		col = 'red'
	else:
		col = 'black'

	#if "AK5" not in opt.algorithm: ax.text(posx, posy, res[0], va='top', ha='left', transform=ax.transAxes)
	#else:
	posy = posy + 0.07
	if 'correction' in changes:
		ax.text(posx, posy - 0.07, res[1], va='top', ha='left', transform=ax.transAxes, color=col)

	if 'algorithm' in changes:
		if "CHS" in changes['algorithm']:
			ax.text(posx, posy - 0.14, r"CHS applied", va='top', ha='left', transform=ax.transAxes, color=col)
		#if "CHS" not in changes['algorithm']:
		#	ax.text(posx, posy - 0.07, r"CHS not applied", va='top', ha='left', transform=ax.transAxes, color=col)

	return ax


def authorlabel(ax, author=None, xpos=0.01, ypos=1.10):
	if author is not None:
		ax.text(xpos, ypos, author, va='top', ha='left',
				transform=ax.transAxes)
	return ax


def datelabel(ax, date='iso', xpos=0.99, ypos=1.10):
	if date is None:
		return ax
	if date == 'now':
		ax.text(xpos, ypos, strftime("%d %b %Y %H:%M", localtime()),
		va='top', ha='right', transform=ax.transAxes)
	elif date == 'today':
		ax.text(xpos, ypos, strftime("%d %b %Y", localtime()),
		va='top', ha='right', transform=ax.transAxes)
	elif date == 'iso':
		ax.text(xpos, ypos, strftime("%Y-%m-%d", localtime()),
		va='top', ha='right', transform=ax.transAxes)
	else:
		ax.text(xpos, ypos, date, va='top', ha='right', transform=ax.transAxes)
	return ax


def binlabel(ax, bin=None, low=0, high=0, xpos=0.03, ypos=0.95, changes={}, color='black'):
	ch_copy = copy.deepcopy(changes)
	if bin is None:
		if 'var' in ch_copy and 'Cut' in ch_copy['var'] and len(ch_copy['var']) > 35:
			ch_copy['var'] = 'var' + ch_copy['var'].split('var')[2]
		if 'bin' in ch_copy:
			ranges = ch_copy['bin'][2:].split('to')
			bin = 'ptz'
			low = int(ranges[0])
			high = int(ranges[1])
		elif 'var' in ch_copy and 'Eta' in ch_copy['var']:
			ranges = ch_copy['var'][11:].replace('_', '.').split('to')
			bin = 'eta'
			low = float(ranges[0])
			high = float(ranges[1])
		elif 'var' in ch_copy and 'PtBin' in ch_copy['var']:
			ranges = ch_copy['var'][10:].replace('_', '.').split('to')
			bin = 'ptbin'
			low = float(ranges[0])
			high = float(ranges[1])
		elif 'var' in ch_copy and 'Npv' in ch_copy['var']:
			ranges = ch_copy['var'][8:].split('to')
			bin = 'Npv'
			low = int(ranges[0])
			high = int(ranges[1])
		elif 'var' in ch_copy and 'Cut' in ch_copy['var']:
			ranges = ch_copy['var'][27:].replace('_', '.')
			bin = 'alpha'
			low = float(ranges)
		else:
			return ax
	if bin == 'ptz':
		text = r"$%u < p_\mathrm{T}^\mathrm{Z} / \mathrm{GeV} < %u$" % (low, high)
	elif bin == 'pthat':
		text = r"$%u < \hat{p}_\mathrm{T} / \mathrm{GeV} < %u$" % (low, high)
	elif bin == 'eta':
		if low == 0:
			text = r"$|\eta_\mathrm{jet}| < %1.3f$" % (high)
		else:
			text = r"$%1.3f < |\eta_\mathrm{jet}| < %1.3f$" % (low, high)
	elif bin == 'alpha':
		text = r"$ \alpha < %1.2f$" % (low)
	elif bin == 'ptbin':
		#text = r"%1.0f < $|\pt_\mathrm{jet}|$ < %1.0f" % (low, high)
		text = r"%1.0f GeV < $p_T^\mathrm{Jet}$ < %1.0f GeV" % (low, high)
	elif bin == 'Npv':
		if low == 0:
			text = r"$ NPV \leq %u$" % (high)
		else:
			text = r"$%u \leq NPV \leq %u$" % (low, high)
	else:
		text = bin
	ax.text(xpos, ypos, text, va='center', ha='left', size='x-large', transform=ax.transAxes, color=color)


def statuslabel(ax, status=None, xpos=0.3, ypos=1.018):
	if (status is not None and hasattr(ax, "statuslabel") and ax.statuslabel is True):
		ax.text(xpos, ypos, r"%s" % status, va='bottom', ha='left',
				transform=ax.transAxes)
		ax.statuslabel = True


def resultlabel(ax, text=None, xpos=0.05, ypos=0.05):
	if text is not None:
		if "\n" in text:
			# two lined thingy
			ypos += 0.07
		ax.text(xpos, ypos, text, va='top', ha='left', transform=ax.transAxes)


def axislabel_2d(ax, y_q, y_obj, x_q='pt', x_obj='Z', brackets=False):
	print "Please use axislabels instead of axislabel_2d."
	return axislabels(ax, x_q, y_q, brackets)


def axislabels(ax, x='zpt', y='events', brackets=False, labels=['', ''], settings=None):
	"""same as the old version, but can handle and and y axis indpendetly

		new idea: improve automatic scaling:
		bottom=0
		top
		autoscaling
		dict = { quantity: axsetting}
	"""

	def setxaxis(limits=(0, 200), quantity="x", unit=""):
		ax.set_xlabel(unitformat(quantity, unit, brackets), ha="right", x=1)
		ax.set_xlim(limits)
		if settings['xlog']:
			ax.set_xscale('log')
			#ax.get_xaxis().get_major_formatter().labelOnlyBase = False
		if settings['xticks'] is not None:
			ax.set_xticks(settings['xticks'])
			ax.get_xaxis().set_major_formatter(plotbase.matplotlib.ticker.ScalarFormatter())

	def setyaxis(limits=(0, 1), quantity="y", unit="", bottom=None):
		string = unitformat(quantity, unit, brackets)
		if settings['ratio'] and y is not "datamcratio":
			string += "   Data/MC ratio"
		if settings['run'] == "diff":
			string += "   Data-MC"
		ax.set_ylabel(string, va="top", y=1)
		if bottom is not None:
			ax.set_ylim(bottom=bottom)
		else:
			ax.set_ylim(limits)

	for quantity, function in zip([x, y], [[setxaxis, ax.set_xticks, ax.set_xticklabels], [setyaxis, ax.set_yticks, ax.set_yticklabels]]):
	# special formatting options for some quantities ...
		if quantity in d_axes:	 # if no special options, read from dictionary
			function[0]((d_axes[quantity][0], d_axes[quantity][1]), d_axes[quantity][2], d_axes[quantity][3])
		else:
			print '"' + quantity + '" is not defined and therefore directly written to label.'
			function[0](quantity=quantity)
	return ax


def getaxislabels_list(quantity, ax=None):
# can we integrate this function somehow into axislabels??
# currently we need one function to change a given ax elemnt and one to simply return limits+label

	labels_list = [0, 1, quantity, ""]
	return labels_list


def unitformat(quantity="", unit="", brackets=False):
	"""Returns a string according to SI standards

		(r"$p_\mathrm{T}$", "GeV") yields "$p_\mathrm{T}$ / GeV"
		brackets are not SI!
		"quantity|unit"
	"""

	if unit != "":
		if "/" in quantity:
			quantity = "(%s)" % quantity
		if "/" in unit:
			unit = "(%s)" % unit
		if brackets:		# units with [] (not allowed by SI system!)
			quantity = r"%s [%s]" % (quantity, unit)
		else:				# units with /
			quantity = r"%s / %s" % (quantity, unit)
	#print "The axis legend string is:", repr(quantity)
	return quantity


def findclosingbracket(string, i=None):
	if i is None:
		i = 0
	count = 0
	while i < len(string):
		#print i, count
		if string[i] == '(':
			count += 1
		elif string[i] == ')':
			count -= 1
		#print "Now count is", count
		if count <= 0:
			return i
		i += 1
	print "Closing bracket not found in", string


def formataxislabels(label):
	"""generic formatter

	to be discussed: what can be combined:
	treequantities: zpt
	constants: pi
	numbers: 1.2
	operators: +-*/ ^
	functions:
		abs, delta[quantity], sqrt, log, exp, sin, cos, ...
"""
	latexvars = ['eta', 'phi', 'theta', 'Delta']
	#functions = ['Deltaphi', 'Deltaeta']
	indices = ['jet1', 'jet2', 'jet3']
	replaced = {
		'TMath::Pi()': '\pi',  # shorthand: pi
		'pt': 'p_\mathrm{T}',
	}

	# high level things:
	for f, r in zip(['abs'], ['|']):
		while f in label:
			i = label.find(f)
			j = i + len(f)
			k = plotbase.findclosingbracket(label, j)
			label = label[:i] + r + label[j + 1:k] + r + label[k + 1:]
	# pure text replacement:
	for v in latexvars:
		label = label.replace(v, '\%s' % v)
	for i in indices:
		label = label.replace(i, '_\mathrm{%s}' % i)
	for r1, r2 in replaced.items():
		label = label.replace(r1, r2)
	label = label.replace('_\mathrm{T}_', '_\mathrm{T}^')
	return label
