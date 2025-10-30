#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "json.hpp"
#include "Eigen/Geometry"

using json = nlohmann::json;

enum JointType {
    REVOLUTE,
    PRISMATIC,
    FIXED
};

struct Joint {
    std::string parent;
    std::string child;
    Eigen::Vector3d axis;
    Eigen::Affine3d origin;
    JointType type;
};

struct Link {
    Eigen::Affine3d origin;
    std::string parent;
    std::vector<std::string> children;
};

struct KinematicTree {
    KinematicTree(std::string file_path);

    std::string root;
    std::unordered_map<std::string, Joint> joints;
    std::unordered_map<std::string, Link> links;
};

KinematicTree::KinematicTree(std::string file_path) {
    /**
     * TODO: Parse the JSON file at file_path and populate the KinematicTree struct.
     * Use the nlohmann::json library to parse the JSON file and Eigen for the
     * Affine3d and Vector3d types. Assume that the JSON file has all of the 
     * required fields and types. If an origin is not provided, set it to the
     * identity matrix.
     * 
     * The joints map should have the joint name as the key and the Joint struct
     * as the value. The links map should have the link name as the key and the
     * Link struct as the value. 
     * 
     * Link::parent should contain the name of the link's parent joint.
     * Link::children should contain the names of the child joints. 
     * Joint::parent should contain the name of the joint's parent link.
     * Joint::child should contain the name of the joint's child link.
     * 
     * As an example, to access the parent Link structure of a joint, you would
     * use:
     * KinematicTree tree("robot.json");
     * Link parent_link = tree.links[tree.joints["joint_name"].parent];
     */
    
    
    std::ifstream file(file_path);
    json j;
    file >> j;
    
    
    auto parse_origin = [](const json& origin_json) -> Eigen::Affine3d {
        Eigen::Affine3d transform = Eigen::Affine3d::Identity();
        
        if (origin_json.contains("xyz")) {
            auto xyz = origin_json["xyz"].get<std::vector<double>>();
            transform.translation() = Eigen::Vector3d(xyz[0], xyz[1], xyz[2]);
        }
        
        if (origin_json.contains("rpy")) {
            auto rpy = origin_json["rpy"].get<std::vector<double>>();
            
            Eigen::AngleAxisd roll(rpy[0], Eigen::Vector3d::UnitX());
            Eigen::AngleAxisd pitch(rpy[1], Eigen::Vector3d::UnitY());
            Eigen::AngleAxisd yaw(rpy[2], Eigen::Vector3d::UnitZ());
            transform.rotate(yaw * pitch * roll);
        }
        
        return transform;
    };
    
    
    for (const auto& link_json : j["links"]) {
        Link link;
        std::string link_name = link_json["name"];
        
        
        link.origin = Eigen::Affine3d::Identity();
        
        
        if (link_json.contains("visual") && link_json["visual"].contains("origin")) {
            link.origin = parse_origin(link_json["visual"]["origin"]);
        }
        
        
        link.parent = "";
        
        links[link_name] = link;
    }
    
    
    std::string first_parent = "";
    for (const auto& joint_json : j["joints"]) {
        Joint joint;
        std::string joint_name = joint_json["name"];
        
        joint.parent = joint_json["parent"];
        joint.child = joint_json["child"];
        
        
        std::string type_str = joint_json["type"];
        if (type_str == "revolute") {
            joint.type = REVOLUTE;
        } else if (type_str == "prismatic") {
            joint.type = PRISMATIC;
        } else {
            joint.type = FIXED;
        }
        
        
        auto axis_vec = joint_json["axis"].get<std::vector<double>>();
        joint.axis = Eigen::Vector3d(axis_vec[0], axis_vec[1], axis_vec[2]);
        
        
        if (joint_json.contains("origin")) {
            joint.origin = parse_origin(joint_json["origin"]);
        } else {
            joint.origin = Eigen::Affine3d::Identity();
        }
        
        joints[joint_name] = joint;
        
        
        
        links[joint.child].parent = joint_name;
        
        
        links[joint.parent].children.push_back(joint_name);
        
        
        if (first_parent.empty()) {
            first_parent = joint.parent;
        }
    }
    
    
    root = "";
    for (const auto& [link_name, link] : links) {
        if (link.parent.empty()) {
            root = link_name;
            break;
        }
    }
    
    
    if (root.empty()) {
        root = first_parent;
    }
}