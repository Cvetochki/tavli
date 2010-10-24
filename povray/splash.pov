#include "colors.inc"
#include "textures.inc"
#include "woods.inc"
#include "dice.inc"
#include "stones1.inc"

//  #include "glass.inc"


global_settings {
      assumed_gamma 1.0
      max_trace_level 5
/*
      photons {
      	spacing 0.01  // higher value 'lower' quality, faster parsing.
      	autostop 0
      	jitter 0.5
      	max_trace_level 15
      }
*/
}

//background { Gray80 }

#declare Slow=false;
#declare ShowPawns=true;

camera {
//	right 150/180
	right x*1//.33333
//	location <0, 2.35, 0>
//	look_at <0.0,0.0,0>
//	location <+.25, 5.05, 0.9	>
//	look_at <+.5,0,-2>

	location <4.5, 2.5, -3>/20
	look_at <0.35,0.025,0>
	angle 50
#if (Slow)
	focal_point <0.35,0.025,0> //right dice in focus
  	aperture 1/32//0.1
	blur_samples 40
#end
}

light_source {
	<-5,5,5> White
//	shadowless
#if (Slow)
	 area_light <4, 0, 0>, <0, 0, 4>, 9, 9
	adaptive 1
	jitter
#end

}

light_source {
	<5,5,5> White
//	shadowless

#if (Slow)
	 area_light <4, 0, 0>, <0, 0, 4>, 9, 9
	adaptive 1
	jitter
#end

}

/*
light_source {
	<-5,5,-5> White
//	shadowless
#if (Slow)
	 area_light <4, 0, 0>, <0, 0, 4>, 9, 9
	adaptive 1
	jitter
#end

}

light_source {
	<5,5,-5> White
//	shadowless

#if (Slow)
	 area_light <4, 0, 0>, <0, 0, 4>, 9, 9
	adaptive 1
	jitter
#end

}
*/
//light_source { <-5,5,-5> White }

#declare tw=1;			//platos tablieras
#declare th=2;			//ypsos tablieras
#declare tthin=0.01;	//pahos tou fylou platosxypsos
#declare wheight=0.15;	//ypsos peribalontos heilous
#declare wthin=0.05;	//pahos peribalontos heilous
#declare hgap=0.003;	//gap between halves

#declare half_tavliera =
	union {
		box {
			<-tw/2			,-tthin			,-th/2>
			<tw/2			,0				, th/2>
		}

		box {
			<-wthin-tw/2	,-tthin			,-th/2>
			<-tw/2			,-tthin+wheight	, th/2>
		}
		box {
			<-tw/2-wthin	,-tthin			,th/2>
			<tw/2			,-tthin+wheight	,th/2+wthin>
		}
		box {
			<+wthin+tw/2	,-tthin			,-th/2>
			<+tw/2			,-tthin+wheight	, th/2+wthin>
		}
		box {
			<-tw/2-wthin	,-tthin			,-th/2>
			<tw/2+wthin		,-tthin+wheight	,-th/2-wthin>
		}
		#declare Index = 0;
		#while(Index <= 5)
		triangle {
			<-tw/2+Index*tw/6,0,-th/2>
			<-tw/2+tw/12+Index*tw/6,0,-th/2+th/3>
			<-tw/2+tw/6+Index*tw/6,0,-th/2>
			texture { Sandalwood }
		}

		triangle {
			<-tw/2+Index*tw/6,0,+th/2>
			<-tw/2+tw/12+Index*tw/6,0,+th/2-th/3>
			<-tw/2+tw/6+Index*tw/6,0,+th/2>
			texture { Sandalwood }
		}
		#declare Index = Index + 1;
		#end

		triangle {
			<-tw/12,0,0>
			<0,0,-tw/6>
			<tw/12,0,0>
			texture { Sandalwood }
		}
		triangle {
			<-tw/12,0,0>
			<0,0,+tw/6>
			<tw/12,0,0>
			texture { Sandalwood }
		}
		texture { T_Wood4 }
	}


#declare tavli =
	union {
		object {half_tavliera translate (-wthin-tw/2-hgap/2)*x}
		object {half_tavliera translate (+wthin+tw/2+hgap/2)*x}
	}

#declare pawn =
	union {
			difference {
				sphere { <0,1,0> tw/12//0.1
					scale <1,0.2,1>
				}
				sphere { <0,1.025,0> tw/12//0.1
					scale <0.7,0.2,0.7>
				}

			}
			cylinder {
				<0, 0.2, 0>,     // Center of one end
				<0, 0.1, 0>,     // Center of other end
				tw/12//0.1            // Radius
				//open           // Remove end caps
				//texture { Sandalwood scale 4 }
			}
			scale <1,0.3,1>
			//texture { T_Grnt15 } //I need to remove color from pawns and declare redpawn and white pawn here

			finish {
				phong 1//0.9
				phong_size 160
				//reflection {0.25}
			}

}

object { tavli }

#if (ShowPawns)

#declare MyRed=color <0.8,0.2,0.2>;
#declare MyWhite=color <0.95,0.95,0.75>;
#declare I=1;
#while (I<6)
object {pawn
	translate <-wthin-hgap/2-tw+tw/12,-0.03,-th/2-tw/12+I*tw/6>
	pigment { MyRed } //Red (5) south-west
}
object {pawn
	translate <-wthin-hgap/2-tw+tw/12,-0.03,th/2+tw/12-I*tw/6>
	pigment { MyWhite } //White (5) north-west
}
object {pawn
	translate <+wthin+hgap/2+tw+tw/12-tw,-0.03,-th/2-tw/12+I*tw/6>
	pigment { MyWhite } //White (5) south-east
}
object {pawn
	translate <+wthin+hgap/2+tw+tw/12-tw,-0.03,th/2+tw/12-I*tw/6>
	pigment { MyRed } //White (5) south-east
}
#if (I<3)
	object {pawn
		translate <+wthin+hgap/2+tw-tw/12,-0.03,th/2+tw/12-I*tw/6>
		pigment {MyWhite} //White (2) upright
	}
	object {pawn
		translate <+wthin+hgap/2+tw-tw/12,-0.03,-th/2-tw/12+I*tw/6>
		pigment {MyRed} //Red (2) updown
	}
#end
#if (I<4)
	object {pawn
		translate <-wthin-hgap/2-tw+tw/12+4*tw/6,-0.03,-th/2-tw/12+I*tw/6>
		pigment {MyWhite} //White (2) upright
	}
	object {pawn
		translate <-wthin-hgap/2-tw-tw/12+5*tw/6,-0.03,th/2+tw/12-I*tw/6>
		pigment {MyRed} //Red (2) updown
	}
#end
#declare I=I+1;
#end



object {pawn
	translate <0.48,-0.03,0>
	pigment {
		MyWhite//White
	}
	finish {
		phong 0.9
		phong_size 60
		reflection {0.25}
	}
}

object {pawn
	translate <0.4,-0.03,0.3>
	pigment {
		MyRed//Red
	}
	finish {
		phong 0.9
		phong_size 60
		reflection {0.25}
	}
}


object {
	dice
	scale 0.025
	rotate 90*x
	translate <0.25,0.025,0>
	finish {
		phong 0.9
		phong_size 60
		reflection {0.15}
		//ambient 0.5
		//diffuse 0.2
	}
}

object {
	dice
	scale 0.025
	rotate <180,30,0>
	translate <0.35,0.025,0>
	finish {
		phong 0.9
		phong_size 60
		reflection {0.15}
		//ambient 0.5
		//diffuse 0.2
	}
}



#end
