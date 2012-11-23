/*

Copyright (c) 2010, Stephan Weiss, ASL, ETH Zurich, Switzerland
You can contact the author at <stephan dot weiss at ieee dot org>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of ETHZ-ASL nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ETHZ-ASL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef BODYVEL_MEASUREMENTS_H
#define BODYVEL_MEASUREMENTS_H

#include <ros/ros.h>
#include <ssf_core/measurement.h>
#include "bodyvel_sensor.h"

class BodyVelMeasurements : public ssf_core::Measurements
{
public:
	BodyVelMeasurements(const ros::NodeHandle & priv_nh=ros::NodeHandle("~"), const ros::NodeHandle & nh=ros::NodeHandle("ssf_core"))
	:Measurements(priv_nh, nh)
	{
		addHandler(new BodyVelSensorHandler(this));

		priv_nh.param("init/p_ci/x", p_ci_[0], 0.0);
		priv_nh.param("init/p_ci/y", p_ci_[1], 0.0);
		priv_nh.param("init/p_ci/z", p_ci_[2], 0.0);

		priv_nh.param("init/q_ci/w", q_ci_.w(), 1.0);
		priv_nh.param("init/q_ci/x", q_ci_.x(), 0.0);
		priv_nh.param("init/q_ci/y", q_ci_.y(), 0.0);
		priv_nh.param("init/q_ci/z", q_ci_.z(), 0.0);
		q_ci_.normalize();
	}

private:

	Eigen::Matrix<double, 3, 1> p_ci_; ///< initial distance camera-IMU
	Eigen::Quaternion<double> q_ci_; ///< initial rotation camera-IMU

	void init(double scale)
	{
		Eigen::Matrix<double, 3, 1> p, v, b_w, b_a, g, w_m, a_m;
		Eigen::Quaternion<double> q, q_wv;
		ssf_core::SSF_Core::ErrorStateCov P;

		g << 0, 0, 9.81;
		b_w << 0,0,0;
		b_a << 0,0.1,-0.6;

		v << 0,0,0;
		w_m << 0,0,0;
		a_m =g;	// zero vel initialization....

		q_wv=Eigen::Quaternion<double>(1, 0, 0, 0);
		q_wv.normalize();
		P << 0.154614609131801, 0.001833435480919, -0.000284773410837, 0.002002361066944, 0.000007889142702, -0.000004964386910, -0.000002123215051, -0.000026177425987, -0.000000079242033, -0.000000393960388, 0.000004988313211, -0.000000033324416, 0.000144055261198, 0.000076980750135, 0.000019909240658, 0.000500330115036, -0.000335608732389, 0.000314769349013, -0.001587895984124, -0.000015531270410, -0.000054383244079, 0.000198792386643, 0.000492300141532, -0.000238744010462, -0.000240715428893
				, 0.002018555431885, 0.164125342651512, -0.002157798050527, -0.000044792384480, 0.002070353446446, -0.000017539420398, 0.000017306922795, -0.000000251511850, -0.000000051450245, -0.000004164685929, 0.000000002103421, 0.000000182866790, -0.000013731897995, 0.000178410974527, 0.000026177387766, 0.001936557901126, -0.002059087106864, 0.000378492584275, 0.001768340270521, -0.000033252459015, -0.000057376733102, -0.000200010795342, 0.000908027206989, 0.000772827423232, 0.000071730628527
				, -0.000325309567891, -0.002122420336860, 0.157541201947975, 0.000003608994269, -0.000002211022132, 0.002052284038053, -0.000004873795860, -0.000007145414007, -0.000000011022212, 0.000000707682656, 0.000000223934515, -0.000000047062882, -0.000084001125050, 0.000030966251107, 0.000312014046157, -0.000096452480284, 0.003054073745056, -0.001274012124349, -0.000218734984063, -0.000235109070301, -0.000209122872406, 0.000003402482272, 0.000213436151897, 0.000238849159080, -0.000213038535661
				, 0.002001286912027, -0.000044627055634, 0.000003721756172, 0.005845508041215, -0.000012943324625, 0.000016496921552, -0.000029306946328, 0.000299894784355, 0.000001559999153, 0.000006445817778, -0.000053464803803, -0.000000903856445, -0.001618260367459, -0.000406034801126, -0.000130162353936, -0.000046296127828, 0.000006278329445, -0.000001411178543, -0.000002611514829, 0.000009749857576, 0.000034576357152, -0.000082424635268, -0.000019715681938, 0.000057116431831, -0.000024541753589
				, 0.000008376096791, 0.002067231136848, -0.000002350749320, -0.000006634033151, 0.005861976223443, -0.000007126394461, -0.000251591915827, -0.000082263669078, 0.000000625790928, 0.000050316698371, 0.000010640461332, -0.000002799627918, -0.000044878652878, -0.002035852152688, -0.000044699738221, 0.000045360926478, -0.000005532942775, 0.000001452127958, 0.000003011145248, 0.000044328178083, -0.000007819719443, -0.000078378934925, -0.000056484635548, -0.000001090312724, -0.000018846000938
				, -0.000005228082199, -0.000017332242836, 0.002052316096582, 0.000016507199549, -0.000007083543973, 0.005557088258617, 0.000002230187670, 0.000037721750447, 0.000000136655991, -0.000000740195120, -0.000003064515417, -0.000000067239906, 0.000533359439500, 0.000097615987828, -0.003829748006085, -0.000019424434333, 0.000002544836770, -0.000000938962086, -0.000001240364053, 0.000068457958837, -0.000091041811980, 0.000005888984968, 0.000020941916178, 0.000014162276145, 0.000002017376802
				, -0.000002155460687, 0.000017860969824, -0.000004838063285, -0.000029640162678, -0.000255446572393, 0.000002237834841, 0.000494395070088, -0.000007515365040, -0.000001778505168, -0.000046776805611, -0.000001861944349, 0.000006886051895, -0.000082277905976, -0.004166689816264, -0.000130757432261, -0.000004527727979, 0.000000607108826, -0.000000078643286, -0.000000191475465, -0.000002060007110, -0.000000331843586, 0.000002218194316, 0.000003597789995, 0.000011324764125, 0.000002956136743
				, -0.000026326051609, -0.000000567223897, -0.000007161746118, 0.000300491823112, -0.000082577222028, 0.000037725020153, -0.000007318347713, 0.000549937763294, 0.000002159475281, -0.000006374997136, -0.000056264870462, -0.000001587259200, 0.004584654548492, 0.000188120044399, 0.000180123206746, -0.000001449978506, 0.000000262231316, -0.000000105010484, -0.000000119162767, 0.000000702780105, 0.000000512424776, -0.000005597557386, -0.000012081757281, 0.000001341852967, 0.000002941913906
				, -0.000000079722227, -0.000000053997844, -0.000000011198399, 0.000001562434406, 0.000000624704114, 0.000000136629987, -0.000001778431481, 0.000002159902955, 0.000000393129477, 0.000000123317249, -0.000000208395750, -0.000000357813977, 0.000018050394372, 0.000015690199434, 0.000001167261350, 0.000000009979281, -0.000000001082311, -0.000000000180923, 0.000000000146933, 0.000000010306094, 0.000000004997607, -0.000000027680185, -0.000000059831284, -0.000000037832480, 0.000000002514330
				, -0.000000388850482, -0.000004140670355, 0.000000706040263, 0.000006499914224, 0.000050985511098, -0.000000736522383, -0.000046892164532, -0.000006359313760, 0.000000123629725, 0.000011781944166, 0.000000334214668, -0.000000539848773, -0.000059182963352, 0.000315313458861, 0.000007447830209, 0.000000252522773, -0.000000031707539, 0.000000018409329, 0.000000020257585, 0.000000392158731, 0.000000000432894, -0.000000851674630, -0.000000242529109, -0.000000162710983, -0.000000432157466
				, 0.000004994743438, 0.000000006453242, 0.000000222097099, -0.000053569762270, 0.000010684142165, -0.000003062735667, -0.000001885058478, -0.000056286032785, -0.000000208426852, 0.000000335421005, 0.000012337335591, 0.000000113290906, -0.000400916497547, 0.000011264171511, -0.000015475341785, 0.000000570361825, -0.000000078401398, 0.000000015105383, 0.000000034059061, -0.000000107682873, -0.000000272014559, 0.000000603066805, 0.000000630286823, -0.000000507049935, -0.000000705300516
				, -0.000000033377064, 0.000000187800651, -0.000000046591479, -0.000000906396003, -0.000002785573932, -0.000000067070992, 0.000006889457619, -0.000001588378428, -0.000000357821908, -0.000000539100097, 0.000000113368062, 0.000002118766721, -0.000012766521660, -0.000059288040018, -0.000002296710309, -0.000000064760332, 0.000000008552586, -0.000000000753570, -0.000000002494330, -0.000000024837858, -0.000000004946918, 0.000000027486740, 0.000000080664102, 0.000000169813776, 0.000000035614492
				, 0.000144451079586, -0.000017097943829, -0.000084248583073, -0.001621552065544, -0.000041803330908, 0.000533399860585, -0.000082974282721, 0.004583586477178, 0.000018049011080, -0.000059356271139, -0.000401016170764, -0.000012775008915, 0.049328520861235, 0.001850519618773, 0.001538221700108, 0.000034020335285, -0.000003877748021, 0.000000416612981, 0.000001715477339, 0.000002245836836, -0.000029987326059, -0.000012917393168, -0.000104496225198, -0.000027447104846, -0.000019761297069
				, 0.000076885398044, 0.000179486292252, 0.000030724369694, -0.000409140930683, -0.002058168235056, 0.000097587545202, -0.004160626108483, 0.000187924483198, 0.000015672973248, 0.000316066570753, 0.000011266039828, -0.000059232331226, 0.001852628164782, 0.046475147270679, 0.001183014791144, 0.000022414985266, -0.000003425783555, -0.000000077489207, 0.000000332782231, -0.000014577235348, 0.000003725980048, 0.000062571093721, 0.000009542908793, -0.000113683387760, 0.000002688498524
				, 0.000020176589549, 0.000026007948820, 0.000311972056493, -0.000130422070169, -0.000045296320670, -0.003829747163997, -0.000130610767531, 0.000180068777325, 0.000001166659130, 0.000007466943270, -0.000015476935698, -0.000002295258428, 0.001538365356098, 0.001182885764061, 0.010253491595035, 0.000019060026855, -0.000002727867987, 0.000000632276412, 0.000001183003645, -0.000035920225499, 0.000073231870816, -0.000006771352736, 0.000042431437615, -0.000027959803808, 0.000000799365266
				, 0.000524140016112, 0.001912605695852, -0.000099978723985, -0.000046253200952, 0.000045352312267, -0.000019420326030, -0.000004531406513, -0.000001397152768, 0.000000010271701, 0.000000248157713, 0.000000568117815, -0.000000065412709, 0.000034398438379, 0.000022798066111, 0.000019063130331, 0.001766014674506, -0.000232376389672, 0.000048982306388, 0.000120211384621, 0.000001377516453, -0.000010933666546, 0.000002087128299, 0.000300369484780, 0.000407842411771, 0.000080373157237
				, -0.000363436058560, -0.002054121700636, 0.003053317085456, 0.000006329981457, -0.000005586714146, 0.000002566644471, 0.000000614510207, 0.000000256056767, -0.000000001141725, -0.000000031362652, -0.000000078783400, 0.000000008737698, -0.000003974909262, -0.000003517403332, -0.000002755218796, -0.000234638208237, 0.018961758969956, 0.002429542774516, 0.000229387369017, 0.000012952685052, 0.000005099371813, -0.000003020797115, -0.000063260247875, -0.000053471827587, -0.000013523106352
				, 0.000320969141437, 0.000370457856464, -0.001270147578837, -0.000001371003419, 0.000001441362152, -0.000000927587574, -0.000000075476161, -0.000000102651921, -0.000000000181894, 0.000000018372277, 0.000000014693075, -0.000000000720806, 0.000000410237171, -0.000000103534700, 0.000000610988270, 0.000048144889532, 0.002430630216959, 0.022923422587565, 0.000875884216090, -0.000001230985253, 0.000016119349949, 0.000006753407728, -0.000008982372984, -0.000001302617704, -0.000000513167272
				, -0.001542309457618, 0.001893235471788, -0.000230373951117, -0.000003198508898, 0.000003598410422, -0.000001487364672, -0.000000254708754, -0.000000124500140, 0.000000000352224, 0.000000022987495, 0.000000041047120, -0.000000003476106, 0.000002264798340, 0.000000698866509, 0.000001427234960, 0.000142978141876, 0.000217296635394, 0.000880897888261, 0.017415526591830, -0.000000683483509, 0.000002683803038, -0.000004555287930, 0.000030185246272, 0.000066286207053, 0.000006551732695
				, -0.000016272034281, -0.000033990676350, -0.000234808984261, 0.000009811550492, 0.000044327948435, 0.000068463900898, -0.000002060326706, 0.000000726631597, 0.000000010488261, 0.000000389006143, -0.000000108632609, -0.000000025398995, 0.000002447944077, -0.000014158213532, -0.000035906199868, 0.000001212925491, 0.000013008903346, -0.000001329962283, -0.000000699849412, 0.005895605354476, 0.000035025289365, -0.000021386155504, 0.000033591164040, -0.000083221069226, 0.000364925798680
				, -0.000054157769157, -0.000056024263615, -0.000207947929937, 0.000034567134617, -0.000007854952511, -0.000091043257327, -0.000000334740167, 0.000000504467100, 0.000000004952297, 0.000000001119941, -0.000000271257544, -0.000000004825162, -0.000029979984458, 0.000003655304096, 0.000073228081531, -0.000010734138324, 0.000005055400739, 0.000016173012621, 0.000002936793560, 0.000035157682271, 0.005852181435321, 0.000052875866932, -0.000111150830232, -0.000044689930924, 0.000203842698291
				, 0.000199121087538, -0.000199803104502, 0.000003625143255, -0.000082479931850, -0.000078217586273, 0.000005896217816, 0.000002157377297, -0.000005580989432, -0.000000027628205, -0.000000835710494, 0.000000605816440, 0.000000028086750, -0.000012741147586, 0.000062015638030, -0.000006786373406, 0.000002040004557, -0.000003229110920, 0.000006680336376, -0.000004397134604, -0.000021450223972, 0.000052847643034, 0.005872501274028, -0.000328373783644, 0.000237516144256, 0.000081321421024
				, 0.000498625518715, 0.000912907830220, 0.000213301804123, -0.000019805942963, -0.000056363581356, 0.000020917723331, 0.000003518811987, -0.000012045432992, -0.000000059451615, -0.000000241958464, 0.000000638255785, 0.000000079814773, -0.000104015956316, 0.000010183052295, 0.000042491962444, 0.000302039358217, -0.000062758641627, -0.000008115344172, 0.000026683460185, 0.000033800002827, -0.000110923321601, -0.000328646964328, 0.005279880846320, -0.000166667118387, 0.000053966963941
				, -0.000233098137537, 0.000757717400004, 0.000239730483392, 0.000057174833960, -0.000001186174058, 0.000014181600269, 0.000011344263687, 0.000001341827636, -0.000000037872657, -0.000000170759269, -0.000000512968429, 0.000000169915435, -0.000027551569101, -0.000113646830347, -0.000027960881607, 0.000406858511459, -0.000052694016456, -0.000002065147035, 0.000060443374690, -0.000083522567583, -0.000045015234988, 0.000236926905164, -0.000165140101279, 0.005112466305056, -0.000159521764716
				, -0.000239185566416, 0.000070861414543, -0.000212970775298, -0.000024615499146, -0.000018912732989, 0.000002017470982, 0.000002930192243, 0.000002941181301, 0.000000002380389, -0.000000426501614, -0.000000704255733, 0.000000036555948, -0.000019540456600, 0.000002182882969, 0.000000786106648, 0.000080465146499, -0.000013012748246, 0.000000258740646, 0.000004969342497, 0.000365169127907, 0.000204285777686, 0.000081516796829, 0.000053500244720, -0.000157958697151, 0.004873324752686;

//		q = ( q_ci_ * q_cv_ *q_wv).conjugate();
		q.setIdentity();
		p = q_wv.conjugate().toRotationMatrix()*p_vc_/scale - q.toRotationMatrix()*p_ci_ ;

		// call initialization in core
		ssf_core_.initialize(p, v, q, b_w, b_a, scale, q_wv, P, w_m, a_m, g, q_ci_, p_ci_);

		ROS_INFO_STREAM("filter initialized to: \n" <<
			"position: [" << p[0] << ", " << p[1] << ", " << p[2] << "]" << std::endl <<
			"scale:" << scale << std::endl <<
			"attitude (w,x,y,z): [" << q.w() << ", " << q.x() << ", " << q.y() << ", " << q.z() << std::endl <<
			"p_ci: [" << p_ci_[0] << ", " << p_ci_[1] << ", " << p_ci_[2] << std::endl <<
			"q_ci: (w,x,y,z): [" << q_ci_.w() << ", " << q_ci_.x() << ", " << q_ci_.y() << ", " << q_ci_.z() << "]");
  }
};

#endif /* BODYVEL_MEASUREMENTS_H */
