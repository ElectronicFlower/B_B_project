$fn=100; // use either 50 or 100 

//Outside dimensions for encclosure box 
width = 38; //x-dimension
height = 25; //z-dimension 
length = 46; //y-dimension

//additional parameters
corner_radius = 3; //radius of corners, higher is more rounded
wall_thickness = 4; //total thickness of each wall 
post_diameter = 10; //support post diameter
hole_diameter = 3;//size for screws if using lid, 0 if not using lid
lid_thickness = 2; //how thick the lid should be, comment out lines 82-99 to skip lid
lid_lip = 2; //to have the lid inset into the box, 0 is flush on top
lid_tolerance = 0.5; //lid is made a little smaller than the opening so it fits easily after printing 

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

difference(){
//BOX
hull(){
    posts(
    x=(width/2 - corner_radius),
    y=(length/2 - corner_radius),
    z=0,
    h=height,
    r=corner_radius);
}

//hollow
hull(){
    posts(
    x=(width/2 - corner_radius - wall_thickness),
    y=(length/2 - corner_radius - wall_thickness),
    z=wall_thickness,
    h=height,
    r=corner_radius);
}

//lip inside of box
hull(){
    posts(
    x=(width/2 - corner_radius - lid_lip),
    y=(length/2 - corner_radius - lid_lip),
    z=(height - lid_thickness),
    h=(lid_thickness + 1),
    r=corner_radius);
}
}

difference(){
////make support posts
posts(
    x=(width/2 - wall_thickness/2 - post_diameter/2),
    y=(length/2 - wall_thickness/2 - post_diameter/2),
    z=wall_thickness-0.5,
    h=height - wall_thickness - lid_thickness + 0.5,
    r=post_diameter/2);
//
////make holes in support posts 
posts(
    x=(width/2 - wall_thickness/2 - post_diameter/2),
    y=(length/2 - wall_thickness/2 - post_diameter/2),
    z=wall_thickness,
    h=height - wall_thickness - lid_thickness + 0.5,
    r=hole_diameter/2);
}

difference(){
//lid
hull(){
    posts(
    x = (width/2 - corner_radius - wall_thickness/2 - lid_tolerance),
    y=(length/2 - corner_radius - wall_thickness/2 - lid_tolerance),
    z=height - lid_thickness +5 , // float ontop of box 
    h=lid_thickness, 
    r=corner_radius);
}
//holes in lid 
posts(
    x=(width/2 - wall_thickness/2 - post_diameter/2),
    y=(length/2 - wall_thickness/2 - post_diameter/2),
    z=height - lid_thickness + 5,
    h=wall_thickness - lid_thickness + 0.5,
    r=hole_diameter/2 + 0.5);
}


//PCB Mock up
translate([-33/2 , -41.61/2, -10]) cube([33,41.61, 1.5748]);
