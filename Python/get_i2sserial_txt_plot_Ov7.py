import numpy as np
import sys, glob, serial

import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib import gridspec
from scipy.fftpack import fft
from matplotlib import rc
from math import pi
import pandas as pd

from datetime import datetime

# TEST NAME
name = "SWEEP_200Hz"

A_weightR = (0,-0.0814392896572664,-0.122788236617123,-0.0662483772058859,0.036025784485616,0.156950855853152,0.281631455473679,0.401481666514987,0.511904111763609,0.61089254604612,0.698036138974231,0.773824116880689,0.839191057433041,0.895239721401245,0.943084150013612,0.983768791394565,1.01823296628476,1.04730086974305,1.07168498913951,1.0919958396508,1.10875403584089,1.12240258187161,1.13331834166726,1.14182225437356,1.14818818484945,1.15265046255614,1.15541023583192,1.15664079337566,1.15649200447139,1.15509401731497,1.1525603380335,1.14899039537672,1.14447167948862,1.13908152843081,1.13288862342241,1.1259542430548,1.11833331781041,1.11007531885128,1.10122500898403,1.09182307875583,1.08190668658217,1.07150991849619,1.06066418040602,1.04939853353294,1.03773998189082,1.02571371918195,1.0133433412622,1.00065102932222,0.987657708101312,0.974383182763341,0.96084625749389,0.947064838405306,0.93305602293989,0.918836177632912,0.904421005820342,0.889825606644168,0.875064526513311,0.860151804012909,0.845101009115746,0.829925277431539,0.814637340129048,0.799249550080752,0.783773904706766,0.768222065932156,0.752605377618151,0.736934880781916,0.721221326879611,0.70547518939342,0.689706673933431,0.673925727039759,0.658142043847735,0.64236507475979,0.6266040312506,0.610867890917233,0.59516540187314,0.579505086573395,0.56389524514863,0.548343958316251,0.532859089929788,0.517448289220369,0.502118992778173,0.486878426316363,0.471733606255225,0.456691341159891,0.441758233061338,0.42694067868685,0.412244870623174,0.397676798432847,0.383242249741767,0.368946811313842,0.354795870126667,0.340794614460351,0.32694803501005,0.313260926031347,0.299737886526296,0.286383321476832,0.273201443131133,0.260196272347641,0.247371640000547,0.234731188449802,0.222278373077984,0.210016463895774,0.197948547217166,0.186077527405086,0.17440612868759,0.162936897044422,0.151672202163337,0.140614239465271,0.129765032197125,0.119126433590699,0.108700129086048,0.0984876386173501,0.0884903189591934,0.0787093661310191,0.069145817857344,0.0598005560812564,0.0506743095285852,0.0417676563200728,0.0330810266287995,0.0246147053800724,0.0163688349909491,0.00834341814653557,0.00053832061019496,-0.00704672593521549,-0.0144121210179399,-0.0215583924826174,-0.0284861935731026,-0.0351962999956258,-0.041689606965062,-0.0479671262370462,-0.0540299831286201,-0.0598794135300493,-0.0655167609103935,-0.0709434733193574,-0.0761611003878838,-0.0811712903298898,-0.085975786947479,-0.0905764266418929,-0.0949751354323927,-0.0991739259851945,-0.1031748946545,-0.106980218537595,-0.110592152545913,-0.114013026493883,-0.117245242207291,-0.120291270652845,-0.123153649090514,-0.125834978250154,-0.128337919533866,-0.130665192245448,-0.132819570848212,-0.134803882252401,-0.136621003133327,-0.138273857281309,-0.139765412984403,-0.141098680444854,-0.142276709230132,-0.143302585759334,-0.144179430825699,-0.14491039715588,-0.145498667006583,-0.145947449799124,-0.146259979792367,-0.146439513794477,-0.146489328913865,-0.146412720349628,-0.146212999221754,-0.145893490441297,-0.145457530620707,-0.144908466024398,-0.144249650559665,-0.143484443807948,-0.142616209096444,-0.141648311610015,-0.140584116543298,-0.139426987292891,-0.138180283689444,-0.136847360269483,-0.135431564586705,-0.133936235562526,-0.132364701875563,-0.130720280389761,-0.129006274620815,-0.12722597324053,-0.12538264861874,-0.123479555402376,-0.121519929131257,-0.119506984890173,-0.117443915996787,-0.1153338927249,-0.113180061062571,-0.110985541504604,-0.108753427878886,-0.106486786206051,-0.104188653591935,-0.101862037152287,-0.0995099129691782,-0.0971352250785614,-0.0947408844884152,-0.0923297682269103,-0.0899047184200274,-0.0874685413980568,-0.0850240068304056,-0.0825738468881382,-0.0801207554336771,-0.0776673872370884,-0.0752163572183806,-0.0727702397152468,-0.0703315677756806,-0.0679028324749034,-0.0654864822560409,-0.0630849222939934,-0.0607005138819488,-0.0583355738399928,-0.0559923739452769,-0.0536731403832114,-0.0513800532191584,-0.0491152458901047,-0.0468808047158072,-0.0446787684289054,-0.0425111277235111,-0.0403798248217879,-0.0382867530580475,-0.0362337564798975,-0.0342226294659841,-0.0322551163598864,-0.0303329111197266,-0.0284576569830737,-0.0266309461467272,-0.0248543194609794,-0.0231292661379685,-0.021457223473741,-0.0198395765836612,-0.0182776581508097,-0.0167727481870324,-0.0153260738063082,-0.0139388090101192,-0.0126120744845188,-0.0113469374086063,-0.0101444112741291,-0.00900545571594701,-0.00793097635310495,-0.0069218246402765,-0.00597879772935028,-0.00510263834094732,-0.00429403464566982,-0.00355362015489517,-0.00288197362094292,-0.00227961894645624,-0.00174702510285223,-0.00128460605771003,-0.00089272071097882,-0.000571672839901945,-0.000321711052566801,-0.000143028750004514,-0.0000357640967765497)
A_weightI = (0,0.0284109147203904,0.205348602802383,0.375736173295909,0.504967000893542,0.593213529174533,0.646150640620442,0.670644230726819,0.67334853964079,0.660042436314287,0.635402796145202,0.603036754532114,0.565633826270163,0.525150476675579,0.482982953842024,0.440111506182063,0.397213356601294,0.354747921613525,0.313019599244499,0.272223344142027,0.232477432749407,0.19384687189304,0.156360050766444,0.120020547179802,0.084815472148415,0.0507213467371966,0.0177082215205971,-0.0142574548345671,-0.0452108569434433,-0.0751874367834621,-0.104222026953035,-0.13234826203175,-0.159598224319512,-0.186002246884871,-0.211588825906229,-0.236384607933987,-0.260414427504877,-0.283701377580315,-0.306266900355929,-0.328130889646933,-0.349311798691525,-0.369826749115055,-0.389691638167587,-0.408921242330126,-0.427529316087486,-0.445528685166251,-0.462931333888197,-0.479748486534871,-0.495990682786837,-0.511667847412436,-0.526789354452077,-0.541364086187527,-0.555400487206291,-0.568906613880047,-0.581890179573676,-0.594358595892767,-0.606319010264546,-0.617778340131982,-0.628743304023633,-0.639220449744962,-0.64921617991905,-0.658736775088291,-0.667788414572246,-0.676377195261784,-0.684509148515187,-0.692190255308821,-0.699426459782306,-0.706223681307158,-0.712587825196983,-0.718524792168108,-0.724040486650422,-0.729140824040379,-0.733831736980646,-0.738119180744263,-0.742009137794976,-0.74550762158993,-0.748620679685684,-0.751354396203984,-0.753714893709388,-0.755708334547006,-0.757340921685012,-0.758618899103433,-0.759548551767585,-0.760136205221971,-0.760388224837775,-0.760311014744918,-0.759911016477426,-0.759194707358922,-0.758168598653282,-0.756839233503774,-0.755213184682463,-0.753297052170251,-0.751097460586571,-0.748621056486497,-0.745874505541913,-0.742864489622284,-0.739597703789555,-0.736080853220789,-0.732320650071266,-0.7283238102899,-0.724097050398116,-0.719647084242548,-0.714980619731242,-0.710104355562425,-0.705024977954232,-0.699749157383274,-0.69428354533931,-0.688634771102839,-0.682809438551865,-0.676814123003684,-0.670655368097072,-0.664339682719819,-0.657873537986196,-0.65126336426853,-0.64451554828672,-0.637636430259178,-0.630632301118362,-0.623509399793763,-0.616273910564893,-0.608931960486578,-0.601489616888569,-0.593952884951239,-0.586327705358915,-0.578619952032131,-0.570835429939919,-0.562979872993015,-0.555058942018682,-0.54707822281768,-0.539043224303714,-0.530959376725558,-0.522832029971881,-0.514666451958668,-0.506467827098989,-0.498241254854741,-0.489991748369872,-0.481724233184491,-0.473443546029139,-0.465154433698439,-0.456861552003211,-0.448569464800091,-0.440282643097582,-0.432005464237427,-0.423742211150101,-0.415497071683176,-0.407274138001251,-0.399077406056094,-0.390910775125597,-0.382778047420102,-0.374682927754627,-0.366629023285491,-0.358619843309796,-0.350658799126229,-0.342749203955594,-0.334894272919505,-0.32709712307562,-0.31936077350783,-0.31168814546977,-0.304082062580053,-0.2965452510676,-0.289080340065459,-0.281689861951499,-0.274376252734392,-0.267141852483281,-0.259988905799562,-0.25291956232921,-0.245935877314114,-0.239039812180869,-0.232233235165536,-0.22551792197286,-0.218895556468478,-0.212367731402685,-0.20593594916431,-0.199601622563326,-0.193366075640809,-0.187230544504897,-0.181196178191449,-0.175264039548089,-0.169435106140401,-0.163710271179018,-0.158090344466422,-0.152576053362274,-0.147168043766138,-0.141866881116485,-0.136673051404902,-0.131586962204452,-0.12660894371118,-0.121739249797764,-0.116978059078375,-0.112325475983803,-0.107781531845978,-0.103346185991009,-0.0990193268399242,-0.0948007730163027,-0.090690274460036,-0.0866875135464781,-0.0827921062102767,-0.0790036030732057,-0.0753214905753491,-0.0717451921090153,-0.06827406915479,-0.0649074224191607,-0.0616444929731785,-0.0584844633916451,-0.0554264588923422,-0.0524695484748471,-0.0496127460585017,-0.0468550116191297,-0.0441952523241207,-0.0416323236655227,-0.0391650305908115,-0.0367921286310251,-0.0345123250259767,-0.0323242798462794,-0.0302266071119415,-0.0282178759073058,-0.0262966114921345,-0.0244612964086553,-0.0227103715844072,-0.021042237430741,-0.0194552549368516,-0.0179477467592317,-0.01651799830646,-0.0151642588192485,-0.0138847424456933,-0.0126776293116877,-0.0115410665864716,-0.0104731695433073,-0.00947202261528338,-0.00853568044626596,-0.00766216893702613,-0.00684948628658768,-0.00609560402885056,-0.00539846806455738,-0.00475599968868182,-0.00416609661332856,-0.00362663398624464,-0.0031354654050527,-0.00269042392732482,-0.00228932307662643,-0.00192995784466649,-0.0016101056896999,-0.00132752753133478,-0.00107996874190488,-0.000865160134574389,-0.000680818948348434,-0.000524649830168922,-0.000394345814280938,-0.000287589299059568,-0.000202053021492681,-0.000135401029518616,-0.0000852896524223076,-0.0000493684694966672,-0.0000252812771791706,-0.0000106670548764348,-0.0000031609296918626,-0.000000395140273398975)
oct_fcenter = (16,20,25,31.5,40,50,63,80,100,125,160,200,250,315,400,500,630,800,1000,1250,1600,2000,2500,3150,4000,5000,6300,8000,10000,12500,16000,20000)
A_weightM = (0,0.0862527563335192,0.239259273016603,0.381531806544967,0.506250460878034,0.613625180667109,0.704859508757902,0.781633681947507,0.845839272839039,0.899358505019223,0.943923283250918,0.981048974915525,1.01201939521761,1.03790037186965,1.05956606575941,1.07772871020298,1.0929669822513,1.10575096638152,1.11646315902413,1.12541568449641,1.13286418812284,1.13901894871629,1.14405372646354,1.14811279599526,1.15131653863897,1.15376589646501,1.15554592906323,1.15672866304896,1.15737538335313,1.15753848293995,1.15726296221303,1.15658764956694,1.15554619914456,1.15416790990674,1.15247840080771,1.15050016963111,1.14825305737783,1.14575463567194,1.14302053116535,1.14006469818355,1.13689964868335,1.13353664686975,1.12998587444893,1.12625657139477,1.12235715622479,1.11829532907191,1.11407816026343,1.10971216665229,1.105203377566,1.10055739192815,1.09577942785218,1.09087436579898,1.08584678621497,1.08070100242552,1.07544108943882,1.07007090921643,1.06459413288358,1.05901426028307,1.05333463721792,1.04755847067931,1.04168884231387,1.03572872035008,1.0296809701729,1.0235483637109,1.01733358777775,1.01103925149202,1.00466789288282,0.998221984775288,0.991703940038196,0.985116116265637,0.978460819956044,0.971740310244169,0.964956802234968,0.958112469982649,0.951209449153056,0.944249839403276,0.937235706508394,0.930169084262097,0.923051976174774,0.915886356990263,0.908674174039997,0.901417348451428,0.894117776225702,0.88677732919808,0.879397855893121,0.87198118228551,0.864529112476192,0.857043429292588,0.84952589482076,0.841978250876608,0.834402219422484,0.826799502935004,0.819171784729265,0.811520729244161,0.803847982293073,0.796155171283764,0.788443905410978,0.780715775824885,0.77297235577824,0.76521520075483,0.757445848581565,0.749665819526331,0.741876616383518,0.734079724548987,0.726276612086027,0.718468729783744,0.710657511209161,0.702844372754208,0.695030713678636,0.687217916149809,0.679407345280239,0.671600349163616,0.663798258910021,0.656002388680949,0.648214035724692,0.640434480412552,0.632664986276338,0.624906800047535,0.617161151698453,0.609429254485683,0.601712304996099,0.594011483195626,0.586327952480973,0.578662859734467,0.571017335382148,0.563392493455199,0.555789431654816,0.548209231420571,0.540652958002308,0.533121660535599,0.525616372120768,0.518138109905483,0.510687875170893,0.503266653421266,0.495875414477122,0.488515112571766,0.481186686451198,0.473891059477313,0.466629139734312,0.459401820138272,0.452209978549744,0.445054477889333,0.437936166256129,0.430855877048914,0.42381442909003,0.416812626751801,0.409851260085418,0.402931104952153,0.396052923156818,0.389217462583339,0.382425457332335,0.375677627860601,0.368974681122364,0.362317310712221,0.355706197009622,0.349142007324811,0.34262539604609,0.336157004788314,0.329737462542503,0.323367385826456,0.317047378836266,0.310778033598636,0.30455993012388,0.298393636559522,0.292279709344378,0.286218693363032,0.280211122100615,0.274257517797774,0.26835839160576,0.262514243741532,0.25672556364279,0.250992830122857,0.245316511525321,0.23969706587836,0.234134941048661,0.228630574894871,0.223184395420489,0.217796820926143,0.21246826016116,0.207199112474384,0.201989767964158,0.196840607627415,0.191752003507812,0.186724318842851,0.181757908209922,0.176853117671216,0.172010284917467,0.16722973941044,0.16251180252415,0.157856787684738,0.153265000508973,0.148736738941332,0.144272293389606,0.139871946859011,0.135535975084739,0.131264646662944,0.127058223180092,0.122916959340667,0.118841103093192,0.114830895754531,0.110886572132444,0.107008360646371,0.103196483446412,0.0994511565304807,0.0957725898596119,0.0921609874713873,0.0886165475914731,0.0851394627432378,0.081729919855436,0.0783881003679385,0.075114180335491,0.0719083305294862,0.0687707165377332,0.0657014988622096,0.062700833014784,0.0597688696108951,0.0569057544611759,0.0541116286610124,0.051386628678026,0.0487308864374704,0.046144529405534,0.0436276806705387,0.0411804590220297,0.0388029790277468,0.0364953511084715,0.0342576816107451,0.0320900728774506,0.0299926233162544,0.0279654274659024,0.026008576060368,0.0241221560908458,0.0223062508655893,0.0205609400675894,0.0188862998100899,0.0172824026899383,0.0157493178387698,0.0142871109720217,0.0128958444357774,0.0115755772514377,0.0103263651582184,0.00914826065347324,0.00804131303084028,0.00700556841621225,0.0060410698015286,0.00514785707638973,0.00432596705749235,0.0035754335158856,0.00289628720204755,0.00228855586878201,0.00175226429193502,0.00128743428893114,0.000894084735129156,0.000572231577997374,0.000321887849108002,0.000143063673951044,0.0000357662795671855)

#ACQUISITION PARAMETERS
Fs = 44100
T = 1.0 / Fs

#VECTORs for RESULTS
index_vector_db = []
rms_py_vector_db = []
rms_ino_vector_db =[]
rms_a_ino_vector_db = []
rms_fft_ino_py_vector_db=[]
rms_afft_ino_py_vector_db=[]
rms_specf_vector_db =[]
rms_Aspecf_py_vector_db =[]
averages_afft_py_vector = []
rms_Aspecf_mod_py_vector_db =[]

def serial_ports():
	"""Lists serial ports

	:raises EnvironmentError:
		On unsupported or unknown platforms
	:returns:
		A list of available serial ports
	"""
	if sys.platform.startswith('win'):
		ports = ['COM' + str(i + 1) for i in range(256)]

	elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
		# this is to exclude your current terminal "/dev/tty"
		ports = glob.glob('/dev/tty[A-Za-z]*')

	elif sys.platform.startswith('darwin'):
		ports = glob.glob('/dev/tty.*')

	else:
		raise EnvironmentError('Unsupported platform')

	result = []

	for port in ports:
		try:
			s = serial.Serial(port)
			s.close()
			result.append(port)
		except (OSError, serial.SerialException):
			pass
	return result

def get_header():
	header = []
	count = 0
	while 1:
		if ser.inWaiting():
			buff_header = ser.readline()
			count = count + 1
			if count == 2 : sB = buff_header.split()
			if '*header_end*' in buff_header: break
	#print sB
	sampleBufferSize = int(sB[0])
	spectrumSize = int(sB[1])
	iteration = int(sB[2])
	mic_type = int(sB[3])
	result = (sampleBufferSize, spectrumSize, iteration, mic_type)
	return result

def get_time_data(time_secs, header, MIC_ALIAS, name, tag):
	#text_file = open("time_domain.txt",'w')
	text_file = open(time_secs+"_"+str(header[0])+"_"+str(header[1])+"_"+str(header[2])+"_"+MIC_ALIAS+"_"+name+"_"+tag+".txt", 'w')
	
	## read serial data from arduino and 
	## write it to the text file
	text_file.write("time" + '\t' + "Serial Data" + '\n')

	time_serial = np.zeros(int(header[0]))
	data_serial = np.zeros(int(header[0]))

	index = -1 

	while 1:
		if ser.inWaiting():
			index = index + 1
			buff_t = ser.readline().decode('utf-8')[:-1]
			if '*time_end*' in buff_t: break
			text_file.write(buff_t)
			text_file.flush()
			try:
				t,d =  buff_t.strip(' \t\n\r').split('\t')
				time_serial[index]=float(t)
				data_serial[index]=float(d)
			except:
				pass

	## close the serial connection and text file
	text_file.close()
	print "done"
	return time_serial, data_serial

def get_freq_data(time_secs, header, MIC_ALIAS, name, tag):
	freq_serial = np.zeros(int(header[1]))
	fftR_serial = np.zeros(int(header[1]))
	fftI_serial = np.zeros(int(header[1]))
	spec_serial = np.zeros(int(header[1]))
	AfftR_serial = np.zeros(int(header[1]))
	AfftI_serial = np.zeros(int(header[1]))
	Aspec_serial = np.zeros(int(header[1]))

	#text_file = open("freq_domain.txt",'w')
	text_file = open(time_secs+"_"+str(header[0])+"_"+str(header[1])+"_"+str(header[2])+"_"+MIC_ALIAS+"_"+name+"_"+tag+".txt", 'w')
	## read serial data from arduino and 
	## write it to the text file
	text_file.write("index" + "\t" + "_fftBufferReal" + "\t" + "_fftBufferImag" + "\t" + "spectrumBuffer" + "\t" + "AfftBufferReal" + "\t" + "AfftBufferImag" + "\t" + "AspectrumBuffer" + '\n')
	index = -1 

	while 1:
		if ser.inWaiting():
			index = index + 1
			buff_f = ser.readline().decode('utf-8')[:-1]
			if '*freq_end*' in buff_f: break
			#print(str(freq) + '\t' + str(fftR) + '\t' + str(fftI) + '\t' + str(spB) + '\t' + str(AfftR) + '\t' +  str(AfftI) + '\t' +  str(AspB))
			#text_file.write(str(freq) + '\t' + str(fftR) + '\t' + str(fftI) + '\t' + str(spB) + '\t' + str(AfftR) + '\t' +  str(AfftI) + '\t' +  str(AspB) + '\n')
			text_file.write(buff_f)
			text_file.flush()
			try:
				fr,fftR,fftI,spec,AfftR,AfftI,Aspec =  buff_f.strip(' \t\n\r').split('\t')
				freq_serial[index]=float(fr)
				fftR_serial[index]=float(fftR)
				fftI_serial[index]=float(fftI)
				spec_serial[index]=float(spec)
				AfftR_serial[index]=float(AfftR)
				AfftI_serial[index]=float(AfftI)
				Aspec_serial[index]=float(Aspec)	
			except:
				pass
	## close the serial connection and text file
	text_file.close()
	return freq_serial, fftR_serial, fftI_serial, spec_serial, AfftR_serial, AfftI_serial, Aspec_serial

def get_scalar_data(time_secs, header, MIC_ALIAS, name, tag):
	## open text file to store the values
	#text_file = open("scalars.txt",'w')
	text_file = open(time_secs+"_"+str(header[0])+"_"+str(header[1])+"_"+str(header[2])+"_"+MIC_ALIAS+"_"+name+"_"+tag+".txt", 'w')
	text_file.write("_datasize"+"\t"+"_rms_time"+"\t"+"_rms_fftB"+"\t"+"rms_specB"+"\t"+"rms_AfftB"+"\t"+"rms_AspecB"+"\n")
	while 1:
		if ser.inWaiting():
			buff_sca = ser.readline().decode('utf-8')[:-1]
			if '*sca_end*' in buff_sca: break
			text_file.write(buff_sca)
			text_file.flush()
			try:
				ds,rmsT,rmsfB,rmssB,rmsafB,rmsasB =  buff_sca.strip(' \t\n\r').split('\t')
				datasize=(float(ds))
				rms_time=(float(rmsT))
				rms_fftB=(float(rmsfB))
				rms_specB=(float(rmssB))
				rms_AfftB=(float(rmsafB))		
				rms_AspecB=(float(rmsasB))		
			except:
				pass
	## close the serial connection and text file
	text_file.close()
	return datasize, rms_time, rms_fftB, rms_specB, rms_AfftB, rms_AspecB

def get_loop_condition():
	while 1:
		if ser.inWaiting():
			buff_loop = ser.readline().decode('utf-8')[:-1]
			if '*continue*' in buff_loop: 
				continue_loop = 1
				break
			elif '*acquisition_end*' in buff_loop:
				continue_loop = 0
				break
	return continue_loop

def freq_to_index(freq, sampleBufferSize,Fs, spectrumSize):
	BandWidth=float(Fs/float(spectrumSize))

	if (freq<BandWidth/2): 
		return 0
	if (freq>(Fs/2-BandWidth/2)): 
		return spectrumSize
	fraction = float(float(freq)/float(Fs))
	i = int(np.round(sampleBufferSize*fraction))
	return i

## ------------------------------------------------------------
## -------------------------- MAIN ----------------------------
## ------------------------------------------------------------

# 1. Retrieve constants (ports, time, header)
ports = serial_ports()
ser = serial.Serial(ports[1], 115200)
print ser

time = (datetime.now())
time_secs = str(time.strftime('%Y_%m_%d-%H%M%S'))
print time_secs

plt.ion()

fig = plt.figure(figsize=(12,8))
#fig.suptitle("Title for whole figure", fontsize=12, fontweight="bold")
fig.patch.set_facecolor('grey')


while 1:
	go_on = get_loop_condition()
	if go_on == 0: 
		print "go_on is 0"
		ser.close()
		break

	header = get_header()
	N = header[0]
	spectrumSize=header[1]
	#MIC PARAMETERS
	TYPE = header[3]
	#print "TYPE"
	#print TYPE

	if TYPE == 1: ## ADAFRUIT
		BIT_LENGTH = 18
		FULL_SCALE_DBFS = 20*np.log10(np.power(2,(BIT_LENGTH))); ##FULL SCALE dBFS
		FULL_SCALE_DBSPL = 120; ##FULL SCALE dBSPL (AOP = 116dB SPL)
		MIC_ALIAS = "ADAFRUIT"
	elif TYPE == 0: ## INVENSENSE
		BIT_LENGTH = 24
		FULL_SCALE_DBFS = 20*np.log10(np.power(2,(BIT_LENGTH))); ##FULL SCALE dBFS
		FULL_SCALE_DBSPL = 120; ##FULL SCALE dBSPL (AOP = 116dB SPL)
		MIC_ALIAS = "INVENSENSE"
	print header

	# 2. Retrieve data (time, freq, scalars)
	x, y = get_time_data(time_secs, header, MIC_ALIAS, name, "_TIME_DOMAIN")

	freq_ino, fftR_ino, fftI_ino, spec_ino, AfftR_ino, AfftI_ino, Aspec_ino = get_freq_data(time_secs, header, MIC_ALIAS, name, "_FREQ_DOMAIN")	

	data_size, rms_y_ino, rms_fft_ino, rms_spec_ino, rms_afft_ino, rms_aspec_ino =get_scalar_data(time_secs, header, MIC_ALIAS, name, "_SCALARS")

	# 3. Perform calculations on retrieved data

	## 3.1 FFT in python
	### 3.1.1 From time domain

	yf = fft(y)
	xf = np.linspace(0.0, 1.0/(2.0*T), N/2)
	specf = abs(yf[0:N/2])/N
	specf[1:N/2] = 2*specf[1:N/2]
	
	y_cor = y+specf[0]
	if TYPE == 1: y_cor[0]=y_cor[1]
	yf = fft(y_cor)
	specf = abs(yf[0:N/2])/N
	specf[1:N/2] = 2*specf[1:N/2]

	# A-weighting in Python (only used for ADAFRUIT)
	AfftR_py = np.zeros(N/2)
	AfftI_py = np.zeros(N/2)
	Aspecf_py = np.zeros(N/2)
	Aspecf_mod_py = np.zeros(N/2)

	for i in range(N/2):
		AfftR_py[i] = np.real(yf[i])* A_weightR[i] 
		AfftI_py[i] = np.imag(yf[i])* A_weightI[i]

	AfftR_pysq = np.zeros(N/2)
	AfftI_pysq = np.zeros(N/2)

	AfftR_pysq = [AfftR_py[i]*AfftR_py[i] for i in range(N/2)]
	AfftI_pysq = [AfftI_py[i]*AfftI_py[i] for i in range(N/2)]

	Aspecf_py = [2*np.sqrt(AfftR_pysq[i]+AfftI_pysq[i])/N for i in range(N/2)]
	Aspecf_py[0] = 1/2*Aspecf_py[0] #Stupid workaround for this. Don't know why it doesnt work

	Aspecf_mod_py = [(specf[i]*A_weightM[i]) for i in range(N/2)]

	## 3.1.2 RMS Results - Python
	rms_y_py = np.sqrt(np.mean(np.power(y_cor,2)))#RMS calculated from time domain signal - python calculation
	rms_fft_py = np.sqrt(np.mean(abs(yf)**2))/np.sqrt(N)#RMS calculated from python's fft - python calculation
	rms_specft_py = np.sqrt(np.mean(specf**2))*np.sqrt(N/2)/np.sqrt(2)#RMS calculated from python's spectrum - python calculation

	# Marked with (1) should be the same
	#print rms_y_py ## (1)
	#print rms_fft_py ## (1)
	#print rms_specft_py ## (1)

	rms_y_py_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_y_py*np.sqrt(2))*20);
	rms_py_vector_db.append(float(rms_y_py_db))
	index_vector_db.append(int(len(rms_py_vector_db)-1))

	rms_fft_py_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_fft_py*np.sqrt(2))*20);
	rms_specf_py_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_specft_py)*20);
	rms_specf_vector_db.append(float(rms_specf_py_db))

	# rms value calculated from afft signal derived from a-weighting in python and the fft in python
	rms_Aspecf_py = 0
	for i in range(len(Aspecf_py)):
		rms_Aspecf_py=rms_Aspecf_py+Aspecf_py[i]*Aspecf_py[i]
	rms_Aspecf_py = np.sqrt(rms_Aspecf_py)/np.sqrt(2)

	rms_Aspecf_mod_py = 0
	for i in range(len(Aspecf_mod_py)):
		rms_Aspecf_mod_py=rms_Aspecf_mod_py+Aspecf_mod_py[i]*Aspecf_mod_py[i]
	rms_Aspecf_mod_py = np.sqrt(rms_Aspecf_mod_py)*np.sqrt(N/2)/np.sqrt(2)
	#print rms_Aspecf_py ## (2)

	rms_Aspecf_py_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_Aspecf_py*np.sqrt(2))*20); # dB SPL values from afft signal derived from a-weighting in python to the fft in arduino - python calculation
	rms_Aspecf_py_vector_db.append(float(rms_Aspecf_py_db)) # vector for plot ax4

	rms_Aspecf_mod_py_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_Aspecf_mod_py*np.sqrt(2))*20); # dB SPL values from afft signal derived from a-weighting in python to the fft in arduino - python calculation
	rms_Aspecf_mod_py_vector_db.append(float(rms_Aspecf_mod_py_db)) # vector for plot ax4

	## 3.2 FFT in arduino
	fftR_inosq = np.zeros(N/2)
	fftI_inosq = np.zeros(N/2)
	specf_fft_ino = np.zeros(N/2)

	fftR_inosq = [fftR_ino[i]*fftR_ino[i] for i in range(N/2)]
	fftI_inosq = [fftI_ino[i]*fftI_ino[i] for i in range(N/2)]

	specf_fft_ino = [2*np.sqrt(fftR_inosq[i]+fftI_inosq[i]) for i in range(N/2)]
	specf_fft_ino[0] = 1/2*specf_fft_ino[0] #Stupid workaround for this. Don't know why it doesnt work

	AfftR_inosq = np.zeros(N/2)
	AfftI_inosq = np.zeros(N/2)
	Aspecf_fft_ino = np.zeros(N/2)

	AfftR_inosq = [fftR_ino[i]*fftR_ino[i]*A_weightR[i]*A_weightR[i] for i in range(N/2)]
	AfftI_inosq = [fftI_ino[i]*fftI_ino[i]*A_weightI[i]*A_weightI[i] for i in range(N/2)]

	Aspecf_fft_ino = [2*np.sqrt(AfftR_inosq[i]+AfftI_inosq[i]) for i in range(N/2)]
	Aspecf_fft_ino[0] = 1/2*Aspecf_fft_ino[0] #Stupid workaround for this. Don't know why it doesnt work

	## 3.3 RMS Results - Arduino SCK

	rms_y_ino_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_y_ino)*20); # dB SPL values from time domain signal in arduino - arduino calculation
	rms_fft_ino_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_fft_ino)*20); # dB SPL values from fft domain signal in arduino - arduino calculation
	rms_spec_ino_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_spec_ino)*20); # dB SPL values from spectrum signal in arduino - arduino calculation
	rms_afft_ino_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_afft_ino)*20); # dBA values from a-fft signal in arduino - arduino calculation
	rms_aspec_ino_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_aspec_ino)*20); #dBa values from a-spectrum in arduino - arduino calculation

	rms_ino_vector_db.append(float(rms_spec_ino_db)) # vector for plot ax4
	rms_a_ino_vector_db.append(float(rms_aspec_ino_db))

	## 3.4 RMS Results - Arduino FFT - Python RMS
	#rms_fft_ino_py = np.sqrt(np.mean(specf_fft_ino**2)) # rms value calculated from fft signal in arduino - python calculation
	rms_fft_ino_py = 0
	for i in range(len(specf_fft_ino)):
		rms_fft_ino_py=rms_fft_ino_py+specf_fft_ino[i]*specf_fft_ino[i]
	rms_fft_ino_py = np.sqrt(rms_fft_ino_py)/np.sqrt(2)

	#print rms_fft_ino_py ## (1)
	
	# rms value calculated from afft signal derived from a-weighting in python to the fft in arduino - python calculation
	rms_Afft_ino_py = 0
	for i in range(len(Aspecf_fft_ino)):
		rms_Afft_ino_py=rms_Afft_ino_py+Aspecf_fft_ino[i]*Aspecf_fft_ino[i]
	rms_Afft_ino_py = np.sqrt(rms_Afft_ino_py)/(np.sqrt(2))

	#print rms_Afft_ino_py ## (2)

	rms_fft_ino_py_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_fft_ino_py)*20); # dB SPL values from from fft signal in arduino - python calculation
	rms_afft_ino_py_db = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-np.log10(rms_Afft_ino_py)*20); # dB SPL values from afft signal derived from a-weighting in python to the fft in arduino - python calculation
	
	rms_fft_ino_py_vector_db.append(float(rms_fft_ino_py_db)) # vector for plot ax4
	rms_afft_ino_py_vector_db.append(float(rms_afft_ino_py_db))

	## 3.4 Relative error
	relative_error_t = (rms_y_py - rms_y_ino)/rms_y_py*100

	## 3.5 Averaging of the samples
	avg_num = 32
	x_oct_avg= np.zeros(avg_num)
	averages_afft_py_vector=np.zeros(avg_num)

	for i in range(0,avg_num):
		avg = 0
		if i==0:
			lowFreq= 0
		else: 
			lowFreq=oct_fcenter[i]/np.sqrt(2)

		hiFreq = oct_fcenter[i]*np.sqrt(2)
		x_oct_avg[i] = hiFreq
		lowBound = int(freq_to_index(lowFreq, N, Fs, spectrumSize))
		hiBound = int(freq_to_index(hiFreq, N, Fs, spectrumSize))

		for j in range(lowBound,hiBound):
			if TYPE ==0: avg +=Aspecf_fft_ino[j]
			if TYPE ==1: avg +=Aspecf_py[j]
		avg /= (hiBound - lowBound + 1)
  		averages_afft_py_vector[i] = avg

	# 4. Plot results
	## 4.1 Create figure
	## 4.2 Define grid
	ax1 = plt.subplot2grid((3, 3), (0, 0), colspan=2)
	ax2 = plt.subplot2grid((3, 3), (1, 0), colspan=2,rowspan=1)
	ax3 = plt.subplot2grid((3, 3), (0, 2))
	ax4 = plt.subplot2grid((3,3),(2,0),colspan=2,rowspan=1)
	ax5 = plt.subplot2grid((3,3),(1,2), rowspan=2)


	plt.subplots_adjust(left=None, bottom=None, right=None, top=None,wspace=None, hspace=0.5)

	## 4.3 Plot actual results 
	### 4.3.1 AX1 TIME DOMAIN SIGNAL PLOT
	ax1.set_xlabel('time [s]',fontsize=8)
	ax1.set_ylabel('Raw signal [-]',fontsize=8)
	ax1.set_title("Time domain signal", fontsize=10, fontweight="bold")
	ax1.grid()

	ax1.plot(x,y_cor, lw=0.5, c='b')

	### 4.3.2 AX2 FREQ DOMAIN SPECTRUM PLOT
	ax2.set_xlabel('Freq [Hz]',fontsize=8)
	ax2.set_ylabel('FFT [-]',fontsize=8)
	ax2.set_title("Frequency domain - Amplitude Spectrum", fontsize=10,fontweight="bold")
	ax2.grid()

	#ax2.plot(xf[0:], np.abs(yf[0:N/2])[0:], lw=0.5, c='r',label='Power Spectrum Pyhton')
	ax2.plot(xf[0:], specf[0:], lw=0.5, c='r',label='Amplitude Spectrum Pyhton')
	if TYPE == 0: ax2.plot(xf[0:], specf_fft_ino[0:],lw=0.5,c='g',label='Amplitude Spectrum FFT Ino')
	if TYPE == 0: ax2.plot(xf[0:], Aspecf_fft_ino[0:],lw=0.5,c='m',label='A-weighted Amplitude Spectrum FFT Ino')
	if TYPE == 1: ax2.plot(xf[0:], Aspecf_py[0:],lw=0.5,c='m',label='A-weighted Amplitude Spectrum FFT Python')
	#ax2.plot(xf[0:], Aspecf_py[0:],lw=0.5,c='c',label='A-weighted Amplitude Spectrum FFT Python')
	ax2.plot(xf[0:], Aspecf_mod_py[0:],lw=0.5,c='m',label='A-weighted Amplitude Spectrum FFT Python')
	ax2.legend(fontsize = 'x-small')

	### 4.3.3 AX3 RMS VALUES (- and dB)
	collabel=("RMS", "dB")
	rowlabel = ("RMS Time py","RMS FFT py","RMS Time Ino","RMS FFT Ino","RMS Sp Ino","Relative error","RMS A-FFT Py","RMS A-FFT Ino")

	cell_text = []
	data = [[rms_y_py,  rms_y_py_db],
	        [rms_fft_py,  rms_fft_py_db],
	        [rms_y_ino,   rms_y_ino_db],
	        [rms_fft_ino,   rms_fft_ino_db],
	        [rms_spec_ino,  rms_spec_ino_db],
	        [relative_error_t, relative_error_t],
	        [rms_Aspecf_py,rms_Aspecf_py_db],
	        [rms_afft_ino,rms_Aspecf_mod_py_db]]

	n_rows=len(data)
	y_offset = np.array([0.0] * len(collabel))
	for row in range(n_rows):
		if row == 5:
			formato = '%1.2f%%'
		else:
			formato = '%1.2f'
		y_offset = data[row]
		cell_text.append([formato % (x) for x in y_offset])

	ax3.axis('tight')
	ax3.axis('off')
	the_table = ax3.table(cellText=cell_text,
		colLabels=collabel,
		rowLabels=rowlabel,
		loc='center',
		bbox=[0.15, 0, 0.85, 1],
		colWidths=[.42,0.42],
		)

	the_table.set_fontsize(8)
	the_table.scale(1,2)

	table_props = the_table.properties()
	table_cells = table_props['child_artists']
	the_table._cells[(1,1)]._text.set_color('red')
	the_table._cells[(1,0)]._text.set_color('red')
	the_table._cells[(2,1)]._text.set_color('red')
	the_table._cells[(2,0)]._text.set_color('red')
	the_table._cells[(5,1)]._text.set_color('green')
	the_table._cells[(5,0)]._text.set_color('green')	
	the_table._cells[(7,1)]._text.set_color('c')	
	the_table._cells[(7,0)]._text.set_color('c')
	the_table._cells[(8,1)]._text.set_color('blue')
	the_table._cells[(8,0)]._text.set_color('blue')

	[i.set_linewidth(0.1) for i in ax1.spines.itervalues()]
	[i.set_linewidth(0.1) for i in ax2.spines.itervalues()]
	[i.set_linewidth(0.1) for i in ax4.spines.itervalues()]

	### 4.3.4 AX4 1/3 octava FFT
	ax4.set_xlabel('Freq [Hz]',fontsize=8)
	ax4.set_ylabel('FFT [-/1000]',fontsize=8)
	ax4.set_title("1/3 Octave FFT", fontsize=10,fontweight="bold")
	ax4.semilogx(x_oct_avg[0:], averages_afft_py_vector[0:],lw=0.5,label='RMS Python Time Domain [dB]')
	#rects1 = ax4.bar(x_oct_avg, averages_afft_py_vector, float(Fs/float(2*spectrumSize)), color='m', antialiased=True, edgecolor='white')
	for i in range(len(x_oct_avg)):
	#		rects1[i].set_height(averages_afft_py_vector[i])
			ax4.text(x_oct_avg[i] + 3, averages_afft_py_vector[i] + .25, str(int(averages_afft_py_vector[i])), fontsize=8,color='k')
	ax4.grid()

	### 4.3.5 AX5 RMS Evolution
	ax5.set_xlabel('Sample Number [#]',fontsize=8)
	ax5.set_ylabel('RMS [dB]',fontsize=8)
	ax5.set_title("RMS values plot", fontsize=10,fontweight="bold")

	ax5.grid()

	ax5.plot(index_vector_db[0:], rms_py_vector_db[0:], marker='.', lw=0.5, c='r',label='RMS Python Time Domain [dB]')
	ax5.plot(index_vector_db[0:], rms_specf_vector_db[0:], marker='x', lw=0.5, c='r',label='RMS Python Freq Domain [dB]')
	if TYPE == 0: ax5.plot(index_vector_db[0:], rms_fft_ino_py_vector_db[0:], marker='.',lw=0.5, c='k',label='RMS Python from Ino FFT [dB]')
	ax5.plot(index_vector_db[0:], rms_Aspecf_py_vector_db[0:], marker='8',markersize=4,lw=0.5, c='c',label='RMS Python from Python A-FFT [dB]')
	if TYPE == 0: ax5.plot(index_vector_db[0:], rms_afft_ino_py_vector_db[0:], marker='x',markersize=4,lw=0.5, c='m',label='RMS Python from Ino FFT(A-python) [dB]')
	#ax5.plot(index_vector_db[0:], rms_afft_ino_py_vector_db[0:], marker='x',markersize=4,lw=0.5, c='m',label='RMS Python from Ino FFT(A-python) [dB]')
	ax5.plot(index_vector_db[0:], rms_ino_vector_db[0:], marker='x',lw=0.5, c='g',label='RMS Arduino [dB]')
	ax5.plot(index_vector_db[0:], rms_Aspecf_mod_py_vector_db[0:], marker='3',lw=0.5, c='b',label='RMS Arduino A-weighted [dBA]')

	ax5.legend(fontsize = 'x-small')
	plt.axis([index_vector_db[max(0,len(index_vector_db)-15)], index_vector_db[len(index_vector_db)-1], 30, 120])

	## 4.4 Show plot
	fig.tight_layout()
	fig.canvas.draw()