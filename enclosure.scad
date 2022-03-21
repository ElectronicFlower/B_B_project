//measurements are in mm 

$fn=50; // use either 50 or 100 

wallthickness=1.32; // this is the wall of the lip, the box is twice as thick

internalx=33+8; // internal x dimension

internaly=41.58+8; // internal y dimension

internalz=1.57+15; // internal overall z dimension

internal_lidz=5; // this is taken of off the internalz for the bottom height and acts as the lid internal height

lip_overlap = internal_lidz/2; // how much overlap between box and lid

lipspacing=0; // this amount is take off of the wall thickness on the lip to increase spacing, if your lid is too tight you can increase this but it will reduce the actual wall thickness on the lip so too much and you will have to increase the wallthickness to compensate. Should be 0 in ideal world, you will likely be better just lightly sanding the lip unless your printer is way out

post_diameter = (3.175);

// INTERNAL VARIABLES, DO NOT MODIFY
internal_botz=internalz-internal_lidz;
actual_y=internaly+(4*wallthickness);
actual_x=internalx+(4*wallthickness);
actual_lidz=internal_lidz+(2*wallthickness);
actual_botz=internal_botz+(2*wallthickness);
lip_x = internalx+(2*wallthickness);
lip_y = internaly+(2*wallthickness);

module bottom() {
    difference() {
        union() {
            scale([actual_x,actual_y,actual_botz])cube(1);
            translate([wallthickness,wallthickness,actual_botz])scale([lip_x,lip_y,lip_overlap])cube(1);
        }
        
        translate([(2*wallthickness),(2*wallthickness),(2*wallthickness)])scale([internalx,internaly,internalz])cube(1);
    }
}

module tabs(){
    tab_thickness=5;
    
    linear_extrude(tab_thickness)
    translate([0,(actual_y),-10])
    polygon([[0,0],[actual_x,0],[31,8],[31,8],[9,8],[9,8]]);

    linear_extrude(tab_thickness)
    rotate([180, 0, 0]) translate([0,0,-10])
    polygon([[0,0],[actual_x,0],[31,8],[31,8],[9,8],[9,8]]);
}



module lid() {
    difference() {
        scale([actual_x,actual_y,actual_lidz+lip_overlap])cube(1);
        
        translate([(2*wallthickness),(2*wallthickness),-.1])scale([internalx,internaly,internal_lidz+lip_overlap+.1])cube(1);
        
        translate([(wallthickness)-lipspacing/2,(wallthickness)-lipspacing/2,-.1])scale([lip_x+lipspacing,lip_y+lipspacing,lip_overlap+.3])cube(1);
    }
}



//vents
//module vents() {
//    translate([0,0,actual_botz + lip_overlap + 10]) polygon([[0,0],[2,0],[2,20],[0,20]]);
//}
module vent(){
    translate([4,4,actual_botz + lip_overlap + 15]) cube([6.3,15.4,10]);
}



module screwHole(r1,h1,r2,h2){
    // r1 rad bottom of cone and the bolt hole
    // r2 rad top of cone
    // h1 full height of screw + cone
    // h2 height of cone

    // shaft first
    cylinder(r=r1,h=h1);
    // cone second
    translate([0,0,h1-h2]) cylinder(r1=r1, r2=r2, h=h2);
}

module DHT(){
    union(){
    color("white") cube([1.2,15.4,25.4]);
    color("white") translate([1.2,0,0]) cube([5.1,15.4,20.1]);
    color("silver") translate([2.7, 3.65, -6.8]) rotate(90,0,0) cube([8.1,.1,6.8]);
    }
}

//Calling the bottom and tabs module and joining then cutting screw holes
color("#ADD8E6") union(){
    bottom();
    difference(){
        tabs();
        //Left Tab Screw Hole 
        translate([20, -5, 0]) screwHole (1,10,1,1);
        // Right Tab Screw Hole
        translate([20, 58, 0]) screwHole (1,10,1,1);
    }
}

difference(){
    color("pink") translate([0,0,actual_botz + lip_overlap + 10])lid();
    vent();
}


//PCB Mock up
module PCB(){
color("green") translate([0 , 0, -5]) cube([33,41.58, 1.57], center);
}
translate ([6,4.2,10]) difference(){
    PCB();
    translate([3.175 , 3.175, -5]) screwHole ((3.175/2),1.6,(3.175/2),1.8);
    translate([28, 3.175, -5]) screwHole ((3.175/2),1.6,(3.175/2),1.8);
    translate([3.175 , 37, -5]) screwHole ((3.175/2),1.6,(3.175/2),1.8);
    translate([28, 37, -5]) screwHole ((3.175/2), 1.6, (3.175/2), 1.8);
}
translate([0,0,45]) DHT();


//supporting posts
module posts(x,y,z,h,r){
    translate([x,y,z]){
    cylinder(r = r, h = h);
    }
    translate([-x, y, z]){
    cylinder(r = r, h = h);
    }
    translate([-x, -y, z]){
    cylinder(r = r, h = h);
    }
    translate([x, -y, z]){
    cylinder(r =r, h = h);
    }
}
translate ([22,25,0]) posts(
    x=(26.67- post_diameter -11),
    y=(35.56- post_diameter -15.5),
    z=wallthickness-0.5,
    h=(actual_botz - wallthickness - internal_lidz ),
    r=((3.175/2)-0.25)
);
